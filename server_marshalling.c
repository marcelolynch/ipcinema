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
		srv_log("[ERROR] Socket opening failed");
		srv_log("Exiting on error");
		free(session);
		exit(3);
	}

	if(socket_bind(session->addr) < 0){
		srv_log("[ERROR] Socket bind failed. Can't continue");
		srv_log("Exiting on error");
		free(session);
		exit(4);
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
	//Cuando hago strlen de no sobrepasarme
	char buf[PACKET_LENGTH + 1] = {0};

	int rd = receive_message(cli->con, buf);
	if(!rd){
		srv_log("[ERROR] Closed socker or error in receive. Closing connection");
		destroy_connection(cli->con);
		free(cli);
		pthread_exit(NULL);
	}

	switch(buf[0]){
		case MOVIE_ADD: 		return req_movie_add(buf);
		case MOVIE_DELETE: 		return req_movie_delete(buf);
		case SCREENING_ADD: 	return req_screening_add(buf);
		case SCREENING_DELETE: 	return req_screening_delete(buf);
		case MAKE_RESERVATION: 	return req_make_reservation(buf);
		case CANCEL_RESERVATION:return req_cancel_reservation(buf);
		case MOVIE_INFO: 		return req_movie_info(buf);
		case MOVIE_SCREENINGS:	return req_movie_screenings(buf);
		case SEATING_INFO:		return req_seating_info(buf);
		case MOVIE_LIST:		return req_movie_list(buf);
		case RESERVATION_LIST:	return req_reservation_list(buf);
		default:
			srv_log("[WARNING] Received unknown request. Ignoring...");
	}

	return NULL;

}


int send_screenings(ClientSession session, ListADT screenings){
	char buf[PACKET_LENGTH] = {0};


	buf[0] = TRANSACTION_BEGIN;
	buf[1] = list_length(screenings);
	
	send_message(session->con, buf);
	receive_message(session->con, buf);

	ListIteratorADT iter = get_iterator(screenings);

	while(buf[0] == TRANSACTION_NEXT && iter_has_next(iter)){
		ScreeningInfo data;

		iter_get_next(iter, &data);

		buf[0] = TRANSACTION_ITEM;
		buf[1] = data.day;
		buf[2] = data.month;
		
		buf[3] = data.slot;
		buf[4] = data.sala;


		strcpy(&buf[5], data.id);

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

	destroy_iterator(iter);
	return 1;
}


//TODO: codigo repetido
int send_movies(ClientSession session, ListADT movies){
	char buf[PACKET_LENGTH] = {0};
	
	char count = list_length(movies);

	buf[0] = TRANSACTION_BEGIN;
	buf[1] = count;
		
	send_message(session->con, buf);
	receive_message(session->con, buf);

	ListIteratorADT iter = get_iterator(movies);

	while(buf[0] == TRANSACTION_NEXT && iter_has_next(iter)){
		MovieInfo info;
		iter_get_next(iter, &info);
	
		buf[0] = TRANSACTION_ITEM;
		strcpy(&buf[1], info.name);
		strcpy(&buf[1+strlen(&buf[1])+1], info.description);

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

	destroy_iterator(iter);

	return 1;
}



int send_tickets(ClientSession session, ListADT tickets){
	char buf[PACKET_LENGTH] = {0};
	
	char count = list_length(tickets);

	buf[0] = TRANSACTION_BEGIN;
	buf[1] = count;
		
	send_message(session->con, buf);
	receive_message(session->con, buf);

	ListIteratorADT iter = get_iterator(tickets);

	while(buf[0] == TRANSACTION_NEXT && iter_has_next(iter)){
		Ticket ticket;
		iter_get_next(iter, &ticket);
	
		buf[0] = TRANSACTION_ITEM;
		buf[1] = ticket.seat;
		buf[2] = ticket.screening.day;
		buf[3] = ticket.screening.month;
		buf[4] = ticket.screening.slot;
		buf[5] = ticket.screening.sala;
		strcpy(&buf[6], ticket.screening.id);

		int len = strlen(&buf[6]);

		strcpy(&buf[6+len+1], ticket.screening.movie);


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

	destroy_iterator(iter);

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