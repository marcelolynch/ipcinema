#include "sockets.h"
#include "client_marshalling.h"
#include "protocol.h"
#include "utilities.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/** 
	Capa de serialización/deserialización del lado del cliente.
	Codifica y decodifica las estructuras para satisfacer el 
	protocolo de comunicación con el servidor.
**/

static int client_send(ClientInstance instance, char * msg);
static int client_rcv(ClientInstance instance, char* buffer);

struct clientInstanceCDT{
	Connection connection;
	int error_state;
};

char buf[PACKET_LENGTH];


ClientInstance new_client(char* srv, int portno){
	ClientInstance instance = failfast_malloc(sizeof(struct clientInstanceCDT));

	Address server = open_socket(srv, portno); 
	Connection con = connect_to_socket(server);

	instance->connection = con;

	free(server);
	return instance;
}

static void clear_buffer(){
	memset(buf, 0, PACKET_LENGTH);
}

static char* error_name(char code){
	switch(code){
		case ERR_NO_SUCH_DATA:		return "asked for non-existent data"; break;
		case ERR_INVALID_PACKET: 	return "protocol violation"; break;
		case ERR_CONSTRAINT_VIOLATION: return "DB constraint violation"; break;
		case ERR_UNKNOWN: 			
		default:					return "unknown"; break;
	}
}

static void wait_ack(ClientInstance instance){
	client_rcv(instance, buf);
	printf("Server replies: ");
	if(buf[0] == ERROR){
		printf("Error %s\n", error_name(buf[1]));
	} else if(buf[0] == OK){
		printf("Ok\n");
	} else {
		printf("Unknown reply\n");
	}

	fflush(stdout);
}

/* Requests al servidor: el primer byte indica el tipo de request,
	los datos se agregan sucevisamente a partir del segundo byte */

/** Las siguientes requieren actualizaciones sobre la base de datos */

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




static void fill_reservation_data(char* buf, ReservationInfo* res){
	// Cuerpo de make_ y cancel_reservation
	
	buf[1] = res->seat;
	strcpy(&buf[2], res->screening_id);
	strcpy(&buf[2 + strlen(&buf[2]) +1], res->client);
}

void make_reservation(ClientInstance instance, ReservationInfo* res){
	clear_buffer();
	buf[0] = MAKE_RESERVATION;
	fill_reservation_data(buf, res);
	client_send(instance,buf);
	wait_ack(instance);
}


void cancel_reservation(ClientInstance instance, ReservationInfo* res){
	buf[0] = CANCEL_RESERVATION;
	fill_reservation_data(buf, res);
	client_send(instance,buf);
	wait_ack(instance);
}



/* 
	Las siguientes piden informacion desde el servidor. El metodo de
	obtenerlas se describe en el protocolo y en server_marshalling.c.
*/

ListADT get_tickets(ClientInstance instance, char* client, int req_cancelled){
	buf[0] = RESERVATION_LIST;
	buf[1] = req_cancelled;
	strcpy(&buf[2], client);

	client_send(instance, buf);
	client_rcv(instance, buf);

	ListADT tickets = new_list(sizeof(Ticket));

	if(!(buf[0] == TRANSACTION_BEGIN)){
		//Algo pasó
		fprintf(stderr, "[WARNING] transaction failed. Returning empty list\n");
		return tickets;
	}

	buf[0] = TRANSACTION_NEXT;
	client_send(instance, buf);
	client_rcv(instance, buf);

	while(buf[0] == TRANSACTION_ITEM){
		Ticket t;
		t.seat = buf[1];
		t.screening.day   = buf[2];
		t.screening.month = buf[3];
		t.screening.slot = buf[4];
		t.screening.sala = buf[5];

		strcpy(t.screening.id, &buf[6]);
		int len = strlen(&buf[6]);
		strcpy(t.screening.movie, &buf[6+len+1]);

		add_to_list(tickets, &t);


		buf[0] = TRANSACTION_NEXT;
		client_send(instance, buf);
		client_rcv(instance, buf);

	}

	if(buf[0] != TRANSACTION_END){
		fprintf(stderr, "[WARNING] Transaction didn't end gracefully: Code %d\n", buf[0]);
	}

	return tickets;
}


ListADT get_screenings(ClientInstance instance, MovieInfo* movie){
	clear_buffer();
	buf[0] = MOVIE_SCREENINGS;
	strcpy(&buf[1], movie->name);

	client_send(instance, buf);
	client_rcv(instance, buf);

	ListADT screenings = new_list(sizeof(ScreeningInfo));

	if(!(buf[0] == TRANSACTION_BEGIN)){
		//Algo pasó
		fprintf(stderr, "[WARNING] transaction failed. Returning empty list\n");
		return screenings;
	}

	buf[0] = TRANSACTION_NEXT;
	client_send(instance, buf);
	client_rcv(instance, buf);

	while(buf[0] == TRANSACTION_ITEM){
		
		ScreeningInfo s;
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
		fprintf(stderr, "[WARNING] Transaction didn't end gracefully: Code %d\n", buf[0]);
	}


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
		fprintf(stderr, "[WARNING] transaction failed. Returning empty list\n");
		return movies;
	}

	buf[0] = TRANSACTION_NEXT;
	
	client_send(instance, buf);
	client_rcv(instance, buf);

	while(buf[0] == TRANSACTION_ITEM){
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
		fprintf(stderr, "[WARNING] Transaction didn't end gracefully: Code %d\n", buf[0]);
	}

	return movies;
}






char * get_hall(ClientInstance instance, char* screening_id){
	buf[0] = SEATING_INFO;
	strcpy(&buf[1], screening_id);

	client_send(instance, buf);
	client_rcv(instance, buf);

	char * seats = failfast_malloc(HALL_ROWS*HALL_COLS);
	if(buf[0] == OK){
		int i;
		for(i = 0; i < HALL_ROWS*HALL_COLS; i++){
			seats[i] = buf[1+i];
		}
	} else {
		return NULL;
	}

	return seats;
}



/* Wrappers para enviar y recibir. Si la conexión se cerró se termina el programa. */


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