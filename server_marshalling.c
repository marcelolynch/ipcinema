#include "server.h"
#include "sockets.h"
#include "errors.h"
#include "server_marshalling.h"
#include "server_logging.h"
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string.h>

struct serverinstanceCDT {
	Address addr;
};


struct clientsessionCDT {
	Connection con;
};

typedef struct serverinstanceCDT* ServerInstance;
typedef struct clientsessionCDT* ClientSession;


char buf[PACKET_LENGTH];

static void clear_buffer(){
	memset(buf, 0, PACKET_LENGTH);
}

ServerInstance server_init(int port){
	ServerInstance session = malloc(sizeof(struct serverinstanceCDT));

	session->addr = open_socket(NULL, port);

	if(session->addr == NULL){
		srv_log("Socket opening failed");
		free(session);
		return NULL;
	}

	if(socket_bind(session->addr) < 0){
		srv_log("Socket binding failed");
		free(session);
		return NULL;
	}

	return session;
} 


ClientSession wait_client(ServerInstance srv){
	Connection c = accept_connection(srv->addr);
	
	if(c == NULL){
		srv_log("[ERROR] Error establishing connection with client");
		return NULL;
	}

	ClientSession cli = malloc(sizeof(struct clientsessionCDT));
	cli->con = c;

	return cli;
}


void end_session(ClientSession session){
	destroy_connection(session->con);
	free(session);
}



ClientRequest * wait_request(ClientSession cli){
	clear_buffer();

	int rd = receive_message(cli->con, buf);
	if(!rd){
		fatal("Error in receive");
	}

	char str[100];
	sprintf(str, "Received request: %d bytes", rd);

	srv_log("Received request");
	ClientRequest * req = malloc(sizeof(*req));
	

	switch(buf[0]){
		case MOVIE_ADD:
		{
			char* name = &buf[1];
			char* desc = &buf[1+strlen(name)+1];

			req->type = REQ_MOVIE_ADD;
			req->data = malloc(sizeof(MovieInfo));
			
			MovieInfo* moviedata = (MovieInfo*)req->data;
			strcpy(moviedata->name, name);
			strcpy(moviedata->description, desc);			
			
			break;
		}
		case MOVIE_DELETE:
		{
			char* name = &buf[1];
			srv_log(str);
			
			req->type = REQ_MOVIE_DELETE;
			req->data = malloc(strlen(name) + 1);
			strcpy((char*)req->data, name);
			
			break;
		}

		case SCREENING_ADD:
		{
			req->type = REQ_SCREENING_ADD;
			req->data = malloc(sizeof(ScreeningInfo));
			ScreeningInfo* info = (ScreeningInfo*)req->data;

			info->day = buf[1];
			info->slot = buf[2];
			strcpy(info->movie, &buf[3]);
			
			break;
		}

		case SCREENING_DELETE:
		{
			req->type = REQ_SCREENING_DELETE;
			req->data = malloc(sizeof(ScreeningInfo));
			ScreeningInfo* info = (ScreeningInfo*)req->data;

			info->day = buf[1];
			info->slot = buf[2];
			break;
		}

		case MAKE_RESERVATION:
		{
			req->type = REQ_MAKE_RESERVATION;
			req->data = malloc(sizeof(ReservationInfo));
			ReservationInfo* info = (ReservationInfo*)req->data;
			info->seat = buf[1];

			info->screening_id = atoi(&buf[2]);

			strcpy(info->client, &buf[2 + strlen(&buf[2]) + 1]);
			break;
		}

		case MOVIE_INFO:
		{
			req->type = REQ_MOVIE_INFO;
			req->data = malloc(strlen(&buf[1] + 1));
			strcpy((char*)req->data, &buf[1]);	
			break;
		}

		case MOVIE_SCREENINGS:
		{	
			req->type = REQ_MOVIE_SCREENINGS;
			req->data = malloc(strlen(&buf[1] + 1));
			strcpy((char*)req->data, &buf[1]);	
			break;
		}
		default:
			srv_log("[WARNING] Received unknown request. Ignoring...");
			break;
	}

	return req;

}


int send_screenings(ClientSession session, ScreeningDataList* screenings){
	clear_buffer();
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
	fflush(stdout);
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

void client_send_error(ClientSession cli){
	clear_buffer();
	buf[0] = ERROR;
	send_message(cli->con, buf);
}

void client_send_ok(ClientSession cli){
	clear_buffer();
	buf[0] = OK;
	send_message(cli->con, buf);
}