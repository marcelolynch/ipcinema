#include "sockets.h"
#include "client_marshalling.h"
#include "protocol.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int client_send(ClientInstance instance, char * msg);
static int client_rcv(ClientInstance instance, char* buffer);

struct clientInstanceCDT{
	Connection connection;
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
	strcpy(buf+1,movie->name);
	strcpy(buf+1+len+1, movie->description);

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
	buf[2] = screening->slot;
	strcpy(buf+3, screening->movie);

	client_send(instance,buf);
	wait_ack(instance);
	printf("Add screening %s", buf);

}


void delete_screening(ClientInstance instance, ScreeningInfo* screening){
	clear_buffer();
	buf[0] = SCREENING_DELETE;
	buf[1] = screening->day;
	buf[2] = screening->slot;

	client_send(instance,buf);
	wait_ack(instance);
	printf("Delete screening %s", buf);

}



MovieInfo request_movie_info(ClientInstance instance, char * movie_name){
	clear_buffer();
	buf[0] = MOVIE_INFO;
	strcpy(buf+1, movie_name);

	client_send(instance, buf);

	//Espero respuesta
	wait_ack(instance);
	printf("Req movie %s", buf);
	MovieInfo info;
	return info;	
}


static int client_send(ClientInstance instance, char * msg){
	return send_message(instance->connection, msg);
}


static int client_rcv(ClientInstance instance, char* buffer){
	return receive_message(instance->connection, buffer);
}