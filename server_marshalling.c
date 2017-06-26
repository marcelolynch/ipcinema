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
		srv_log("Error establishing connection with client");
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


void client_send(ClientSession client, char* msg){
    send_message(client->con, msg);

}


ClientRequest * wait_request(ClientSession cli){
	char buf[PACKET_LENGTH] = {0};

	int rd = receive_message(cli->con, buf);
	if(!rd){
		fatal("Error in receive");
	}

	char str[100];
	sprintf(str, "Received request: %d bytes", rd);

	srv_log(str);
	ClientRequest * req = malloc(sizeof(*req));
	

	switch(buf[0]){
		case MOVIE_ADD:
		{
			char* name = buf+1;
			char* desc = buf+1+strlen(name)+1;
			srv_log("Movie add request");
			req->type = MOVIE_ADD;
			break;
		}
		case MOVIE_DELETE:
		{
			srv_log("Movie delete request");
			break;
		}

		case SCREENING_ADD:
		{
			srv_log("Screening add request");
			printf("PRING\n\n\n\n\n\n\n\n");
			break;
		}

		case SCREENING_DELETE:
		{
			srv_log("Screening delete request");
			break;
		}

		default:
			srv_log("Unknown request");
			break;
	}

	return req;

}