#include "sockets.h"
#include "client_marshalling.h"
#include <stdlib.h>

#define SERVER_PORT 12345 


struct clientInstanceCDT{
	Connection connection;
};


ClientInstance new_client(char* srv){
	ClientInstance instance = malloc(sizeof(struct clientInstanceCDT));

	Address server = open_socket(srv, SERVER_PORT); 
	Connection con = connect_to_socket(server);

	instance->connection = con;

	return instance;
}


void client_send(ClientInstance instance, char * msg){
	send_message(instance->connection, msg);
}


