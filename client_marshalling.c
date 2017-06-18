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
	char reserve[]
	
}


void client_send(ClientInstance instance, char * msg){
	send_message(instance->connection, msg);
}


void client_rcv(ClientInstance instance, char* buf){
	receive_message(instance->connection, buf);
}