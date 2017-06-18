#include "server.h"
#include "sockets.h"
#include "errors.h"
#include "server_marshalling.h"

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



ServerInstance server_init(char* hostname, int port){
	ServerInstance session = malloc(sizeof(struct serverinstanceCDT));

	session->addr = open_socket(hostname, port);
	socket_bind(session->addr);

	return session;
} 


ClientSession wait_client(ServerInstance srv){
	ClientSession cli = malloc(sizeof(struct clientsessionCDT));
	cli->con = accept_connection(srv->addr);
	
	return cli;		
}


void client_send(ClientSession client, char* msg){
    send_message(client->con, msg);

}


ClientRequest * wait_request(ClientSession cli){
	char buf[BUF_SIZE] = {0};

	if(!receive_message(cli->con, buf)){
		fatal("Error in receive");
	}

	printf("received: %s\n", buf);
	ClientRequest * req = malloc(sizeof(*req));

	if(strncmp(buf, "reserve", strlen("reserve")) == 0){
		req->type = RESERVATION;
		req->data = malloc(sizeof(*req->data));
		ReservationRequest r = req->data;
		int code = atoi(buf+strlen("reserve")+1);
		r->auditorium = code/100;
		r->seat = code%100;

	} else if(strncmp(buf, "info", strlen("info")) == 0){

		req->type = SEAT_INFO;
		req->data = malloc(sizeof(*req->data));

		AuditoriumInfo r = req->data;
		int code = atoi(buf+strlen("info")+1);
		r->auditorium = code;
	} else{
		free(req);
		return NULL;
	}

	return req;

}