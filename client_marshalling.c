#include "sockets.h"
#include "client_marshalling.h"
#include "protocol.h"

#include <stdlib.h>



struct clientInstanceCDT{
	Connection connection;
};


ClientInstance new_client(char* srv, int portno){
	ClientInstance instance = malloc(sizeof(struct clientInstanceCDT));

	Address server = open_socket(srv, portno); 
	Connection con = connect_to_socket(server);

	instance->connection = con;

	return instance;
}


void reserveSeat(int room, int seat){
}


int client_send(ClientInstance instance, char * msg){
	return send_message(instance->connection, msg);
}


int client_rcv(ClientInstance instance, char* buf){
	return receive_message(instance->connection, buf);
}