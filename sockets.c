#include "sockets.h"
#include "errors.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>


#define MAX_CONNECTIONS 10
#define BUF_SIZE 1024

struct AddressCDT{
	int sockfd;
	struct sockaddr_in addr;
};


struct ConnectionCDT{
	int fd;
};




Address open_socket(char* hostname, int port){
	Address con = malloc(sizeof(struct AddressCDT));

	struct hostent* h = gethostbyname(hostname);

	con->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bzero((char *) &con->addr, sizeof(con->addr));

    con->addr.sin_family = AF_INET;
    memcpy(&con->addr.sin_addr.s_addr, h->h_addr, h->h_length);
    con->addr.sin_port = htons(port);

    return con;

}


int socket_bind(Address a){
	    if (bind(a->sockfd, (struct sockaddr *)&a->addr,
              sizeof(a->addr)) < 0){
    		fatal("Bind failed");
          }
    
    if(listen(a->sockfd,MAX_CONNECTIONS) < 0){
    	fatal("listen failed");
    }

}


Connection accept_connection(Address addr){
	Connection con = malloc(sizeof(struct ConnectionCDT));
	int s = accept(addr->sockfd, 0, 0);
	if(s < 0){
		fatal("Accept failed");
	}
	con->fd = s;
	return con;
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
	return read(con->fd, buf, BUF_SIZE); /* read file name from socket */
}

int send_message(Connection con, char* buf){
	return write(con->fd, buf, strlen(buf));
}


