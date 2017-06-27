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
			char* name = buf+1;
			char* desc = buf+1+strlen(name)+1;
			sprintf(str, "Movie add request: %s. Desc: %s", name, desc);
			srv_log(str);
			req->type = REQ_MOVIE_ADD;
			req->data = malloc(sizeof(MovieInfo));
			MovieInfo* moviedata = (MovieInfo*)req->data;
			strcpy(moviedata->name, name);
			strcpy(moviedata->description, desc);			
			break;
		}
		case MOVIE_DELETE:
		{
			char* name = buf+1;
			sprintf(str, "Movie delete request: %s", name);
			srv_log(str);
			break;
		}

		case SCREENING_ADD:
		{
			sprintf(str, "Screening add request for movie %s, day %d, slot %d",buf+3, buf[1], buf[2]);
			srv_log(str);
			break;
		}

		case SCREENING_DELETE:
		{
			sprintf(str, "Screening delete request for day %d, slot %d", buf[1], buf[2]);
			srv_log(str);
			break;
		}

		default:
			srv_log("[WARNING] Received unknown request. Ignoring...");
			break;
	}

	return req;

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