#include "server.h"
#include "sockets.h"
#include "errors.h"
#include "server_marshalling.h"
#include "server_logging.h"
#include "protocol.h"
#include "request_constructor.h"
#include "utilities.h"



#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string.h>

struct serverinstanceCDT {
	Address addr;
};


struct clientsessionCDT {
	Connection con;
	char status;
};

typedef struct serverinstanceCDT* ServerInstance;
typedef struct clientsessionCDT* ClientSession;


ServerInstance server_init(int port){
	ServerInstance session = failfast_malloc(sizeof(struct serverinstanceCDT));

	session->addr = open_socket(NULL, port);

	if(session->addr == NULL){
		srv_log("Socket opening failed");
		srv_log("Exiting on error");
		free(session);
		exit(1);
	}

	if(socket_bind(session->addr) < 0){
		srv_log("[ERROR] Socket bind failed. Can't continue");
		srv_log("Exiting on error");
		free(session);
		exit(1);
	}

	return session;
} 


ClientSession wait_client(ServerInstance srv){
	Connection c = accept_connection(srv->addr);
	
	if(c == NULL){
		srv_log("[ERROR] Error establishing connection with client");
		return NULL;
	}

	ClientSession cli = failfast_malloc(sizeof(struct clientsessionCDT));
	cli->con = c;

	return cli;
}


void end_session(ClientSession session){
	destroy_connection(session->con);
	free(session);
}



ClientRequest * wait_request(ClientSession cli){
	//El buffer termina con un cero para protegerme
	//Cuando hago strlen de no sobrepasarme del buffer
	char buf[PACKET_LENGTH + 1] = {0};

	int rd = receive_message(cli->con, buf);
	if(!rd){
		srv_log("Error in receive. Closing connection");
		destroy_connection(cli->con);
		free(cli);
		pthread_exit(NULL);
	}

	char str[100];
	sprintf(str, "Received request: %d bytes", rd);

	srv_log("Received request");

	switch(buf[0]){
		case MOVIE_ADD: 		return movie_add(buf);
		case MOVIE_DELETE: 		return movie_delete(buf);
		case SCREENING_ADD: 	return screening_add(buf);
		case SCREENING_DELETE: 	return screening_delete(buf);
		case MAKE_RESERVATION: 	return make_reservation(buf);
		case MOVIE_INFO: 		return movie_info(buf);
		case MOVIE_SCREENINGS:	return movie_screenings(buf);
		case SEATING_INFO:		return seating_info(buf);
		case MOVIE_LIST:		return movie_list(buf);
		case RESERVATION_LIST:	return reservation_list(buf);
		default:
			srv_log("[WARNING] Received unknown request. Ignoring...");
	}

	return NULL;

}


int send_screenings(ClientSession session, ScreeningDataList* screenings){
	char buf[PACKET_LENGTH] = {0};
	char count = 0;

	ScreeningDataList* runner = screenings;
	
	while(runner != NULL){
		count++;
		runner = runner->next;
	}

	buf[0] = TRANSACTION_BEGIN;
	buf[1] = count;
	
	send_message(session->con, buf);
	receive_message(session->con, buf);

	while(buf[0] == TRANSACTION_NEXT && screenings != NULL){
		ScreeningDataList* aux;
	
		buf[0] = TRANSACTION_ITEM;
		buf[1] = screenings->data.day;
		buf[2] = screenings->data.slot;
		buf[3] = screenings->data.sala;


		strcpy(&buf[4], screenings->data.id);

		aux = screenings;
		screenings = screenings->next;
		free(aux);
		send_message(session->con, buf);
		receive_message(session->con, buf);

	}
	
	if(buf[0] == TRANSACTION_NEXT){
		buf[0] = TRANSACTION_END;
		send_message(session->con, buf);	
	} else {
		//El cliente cerro la conexion, mando acknowledgement
		srv_log("Weird shit\n");
		buf[0] = OK;

		send_message(session->con, buf);
	}

	return 1;
}


//TODO: codigo repetido
int send_movies(ClientSession session, MovieInfoList* movies){
	char buf[PACKET_LENGTH] = {0};
	char count = 0;

	MovieInfoList* runner = movies;
	
	while(runner != NULL){
		count++;
		runner = runner->next;
	}

	buf[0] = TRANSACTION_BEGIN;
	buf[1] = count;
		
	send_message(session->con, buf);
	receive_message(session->con, buf);

	while(buf[0] == TRANSACTION_NEXT && movies != NULL){
		MovieInfoList* aux;
	
		buf[0] = TRANSACTION_ITEM;
		strcpy(&buf[1], movies->info.name);
		strcpy(&buf[1+strlen(&buf[1])+1], movies->info.description);

		aux = movies;
		movies = movies->next;
		free(aux);

		send_message(session->con, buf);
		receive_message(session->con, buf);
	}
	
	if(buf[0] == TRANSACTION_NEXT){
		buf[0] = TRANSACTION_END;
		send_message(session->con, buf);	
	} else {
		//El cliente cerro la conexion, mando acknowledgement
		srv_log("Weird shit\n");
		buf[0] = OK;
		send_message(session->con, buf);
	}

	return 1;
}


int send_seats(ClientSession session, char* seats){
	char buf[PACKET_LENGTH];
	buf[0] = OK;
	memcpy(&buf[1], seats, 100);
	send_message(session->con, buf);
	return 1;
}

static char get_error_code(error_t error){
	switch(error){
		case INVALID_REQUEST: return ERR_INVALID_PACKET;
		
		default: return ERR_UNKNOWN;	
	}
}

void client_send_error(ClientSession cli, error_t error){
	char buf[PACKET_LENGTH];
	buf[0] = ERROR;
	buf[1] = get_error_code(error);
	send_message(cli->con, buf);
}



void client_send_ok(ClientSession cli){
	char buf[PACKET_LENGTH] = {0};
	buf[0] = OK;
	send_message(cli->con, buf);
}