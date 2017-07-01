#include "sockets.h"
#include "errors.h"
#include "protocol.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>


#define MAX_CONNECTIONS 10

struct AddressCDT{
	int sockfd;
	struct sockaddr_in addr;
};


struct ConnectionCDT{
	int fd;
};



Address open_socket(char* hostname, int port){

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(fd < 0){
		return NULL;
	}

	Address con = malloc(sizeof(struct AddressCDT));
	con->sockfd = fd;	
 
 	//Clear de la estructura por las dudas
	memset((char *)&con->addr, 0, sizeof(con->addr));
    
    con->addr.sin_family = AF_INET;
    con->addr.sin_port = htons(port);
	

	if(hostname == NULL){
		//Si hostname es NULL, usar INADDR_ANY
		con->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else{
		//Utilizar el host indicado
		struct hostent* h = gethostbyname(hostname);
		if(h == NULL){
			free(con);
			return NULL;
		}
	    memcpy(&con->addr.sin_addr.s_addr, h->h_addr, h->h_length);
	}

    return con;
}


int socket_bind(Address a){

    if (bind(a->sockfd, (struct sockaddr *)&a->addr, sizeof(a->addr)) < 0){
    	return -1;
    }
    
    if(listen(a->sockfd, MAX_CONNECTIONS) < 0){
    	return -2;
    }

    int yes = 1;
	if (setsockopt(a->sockfd,SOL_SOCKET,SO_REUSEADDR,&yes, sizeof(yes)) == -1) {
   		return -3;
	} 


    return 0;
}


Connection accept_connection(Address addr){
	int s = accept(addr->sockfd, 0, 0);
	if(s < 0){
		return NULL;
	}
	Connection con = malloc(sizeof(struct ConnectionCDT));
	con->fd = s;
	return con;
}


void destroy_connection(Connection c){
	close(c->fd);
	free(c);
}


Connection connect_to_socket(Address addr){
	Connection con = malloc(sizeof(struct ConnectionCDT));
	
	con->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(connect(con->fd, (struct sockaddr*)&addr->addr, sizeof(addr->addr)) != 0){
		fatal("Connect failed");
	}

	return con;
}



int receive_message(Connection con, char* buf){
	int r = read(con->fd, buf, PACKET_LENGTH); /* read from socket */
	return r;
}

int send_message(Connection con, char* buf){
	return send(con->fd, buf, PACKET_LENGTH, MSG_NOSIGNAL);
}