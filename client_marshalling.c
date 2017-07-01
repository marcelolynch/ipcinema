#include "sockets.h"
#include "client_marshalling.h"
#include "protocol.h"
#include "utilities.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int client_send(ClientInstance instance, char * msg);
static int client_rcv(ClientInstance instance, char* buffer);

struct clientInstanceCDT{
	Connection connection;
	int error_state;
};

char buf[PACKET_LENGTH];


ClientInstance new_client(char* srv, int portno){
	ClientInstance instance = malloc(sizeof(struct clientInstanceCDT));

	Address server = open_socket(srv, portno); 
	Connection con = connect_to_socket(server);

	instance->connection = con;

	return instance;
}

static void clear_buffer(){
	memset(buf, 0, PACKET_LENGTH);
}


static void wait_ack(ClientInstance instance){
	client_rcv(instance, buf);
	printf("Server replies: ");
	if(buf[0] == ERROR){
		printf("Error\n");
	} else if(buf[0] == OK){
		printf("Ok\n");
	} else {
		printf("Unknown reply\n");
	}

	fflush(stdout);
}

void add_movie(ClientInstance instance, MovieInfo* movie){
	clear_buffer();
	buf[0] = MOVIE_ADD;
	int len = strlen(movie->name);
	strcpy(&buf[1],movie->name);
	strcpy(&buf[1+len+1], movie->description);

	client_send(instance, buf);
	wait_ack(instance);
}


void delete_movie(ClientInstance instance, char* movie_name){
	clear_buffer();
	buf[0] = MOVIE_DELETE;
	strcpy(buf+1, movie_name);
	client_send(instance, buf);
	wait_ack(instance);

}


void add_screening(ClientInstance instance, ScreeningInfo* screening){
	clear_buffer();
	buf[0] = SCREENING_ADD;
	buf[1] = screening->day;
	buf[2] = screening->month;
	buf[3] = screening->slot;
	buf[4] = screening->sala;
	strcpy(&buf[5], screening->movie);

	client_send(instance,buf);
	wait_ack(instance);

}


void delete_screening(ClientInstance instance, ScreeningInfo* screening){
	clear_buffer();
	buf[0] = SCREENING_DELETE;
	buf[1] = screening->day;
	buf[2] = screening->day;
	buf[3] = screening->slot;

	client_send(instance,buf);
	wait_ack(instance);

}


void make_reservation(ClientInstance instance, ReservationInfo* res){
	clear_buffer();
	buf[0] = MAKE_RESERVATION;
	buf[1] = res->seat;
	strcpy(&buf[2], res->screening_id);
	strcpy(&buf[2 + strlen(&buf[2]) +1], res->client);

	client_send(instance,buf);
	wait_ack(instance);

}


MovieInfo request_movie_info(ClientInstance instance, char * movie_name){
	clear_buffer();
	buf[0] = MOVIE_LIST;
	strcpy(buf+1, movie_name);

	client_send(instance, buf);

	//Espero respuesta
	wait_ack(instance);
	MovieInfo info;
	return info;	
}



ListADT get_screenings(ClientInstance instance, MovieInfo* movie){
	clear_buffer();
	buf[0] = MOVIE_SCREENINGS;
	strcpy(&buf[1], movie->name);

	client_send(instance, buf);
	client_rcv(instance, buf);

	ListADT screenings = new_list(sizeof(ScreeningData));
	//TODO malloc

	if(!(buf[0] == TRANSACTION_BEGIN)){
		//Algo pasó
		printf("Screenings transaction failed: received %d\n", buf[0]);
		destroy_list(screenings);
		return NULL;
	}

	int trans_length = buf[1];
	

	buf[0] = TRANSACTION_NEXT;
	client_send(instance, buf);
	client_rcv(instance, buf);

	int i = 0;
	while(buf[0] == TRANSACTION_ITEM){
		if(i >= trans_length){
			printf("Error in length");
			break;
		}	
		i++;
		
		ScreeningData s;
		s.day = buf[1];
		s.month = buf[2];
		s.slot = buf[3];
		s.sala = buf[4];
		strcpy(s.id, &buf[5]);
		add_to_list(screenings, &s);

		buf[0] = TRANSACTION_NEXT;
		client_send(instance, buf);
		client_rcv(instance, buf);

	}

	if(buf[0] != TRANSACTION_END){
		printf("Something happened in the transaction: received %d\n", buf[0]);
	}

	wait_ack(instance);

	return screenings;
}


ListADT get_movies(ClientInstance instance){
	clear_buffer();
	buf[0] = MOVIE_LIST;

	client_send(instance, buf);
	client_rcv(instance, buf);

	ListADT movies = new_list(sizeof(MovieInfo));

	if(!(buf[0] == TRANSACTION_BEGIN)){
		//Algo pasó
		printf("Screenings transaction failed: received %d\n", buf[0]);
		destroy_list(movies);
		return NULL;
	}

	int trans_length = buf[1];

	buf[0] = TRANSACTION_NEXT;
	
	client_send(instance, buf);
	client_rcv(instance, buf);

	int i = 0;

	while(buf[0] == TRANSACTION_ITEM){
		if(i >= trans_length){
			//TODO: Borrar esto, debug
			printf("Error in length");
			break;
		}
		i++;

		MovieInfo m;
		safe_strncpy(m.name, &buf[1], MOVIE_NAME_LEN);
		
		int len = strlen(&buf[1]);
		
		safe_strncpy(m.description, &buf[1 + len + 1], DESCRIPTION_LEN);
		
		add_to_list(movies, &m);
		
		buf[0] = TRANSACTION_NEXT;
		client_send(instance, buf);
		client_rcv(instance, buf);

	}

	if(buf[0] != TRANSACTION_END){
		printf("Something happened in the transaction: received %d\n", buf[0]);
	}

	wait_ack(instance);

	return movies;
}






char * get_hall(ClientInstance instance, char* screening_id){
	buf[0] = SEATING_INFO;
	strcpy(&buf[1], screening_id);

	client_send(instance, buf);
	client_rcv(instance, buf);

	char * seats = malloc(HALL_ROWS*HALL_COLS);
	if(buf[0] == OK){
		int i;
		for(i = 0; i < HALL_ROWS*HALL_COLS - 1 ; i++){
			seats[i] = buf[1+i];
		}
	} else {
		//TODO: Error
		return NULL;
	}

	wait_ack(instance);
	return seats;
}



static int client_send(ClientInstance instance, char * msg){
	int i;
	if((i = send_message(instance->connection, msg)) <= 0){
		fprintf(stderr, "Connection error. Can't continue\n");
		destroy_connection(instance->connection);
		free(instance);
		exit(1);
	}
	return i;
}


static int client_rcv(ClientInstance instance, char* buffer){
	int i;
	if((i = receive_message(instance->connection, buffer)) <= 0){
		fprintf(stderr, "Connection with server closed. Can't continue\n");
		destroy_connection(instance->connection);
		free(instance);
		exit(1);
	}
	return i;
}