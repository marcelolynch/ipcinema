#include "errors.h"
#include "client_marshalling.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h> 


int main(int argc, char*argv[]){
  
  if (argc != 3) fatal("Usage: client server-name server-port-number");

	ClientInstance cli = new_client(argv[1], atoi(argv[2]));

	char buf[1024];
	while(1){
		memset(buf, 0, 1024); 
		fgets(buf,255,stdin);
		client_send(cli, buf);
		client_rcv(cli, buf);
		printf("%s", buf);
	}

}
