#include "client_marshalling.h"

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h> 

// ui.c
int start_ui(ClientInstance inst);


int main(int argc, char*argv[]){

	//Para manejar yo lo errores de broken pipes
	signal(SIGPIPE, SIG_IGN);

  
  	if (argc != 3){
  		fprintf(stderr, "Usage: client server-name server-port-number");
  		exit(1);
  	}

	ClientInstance cli = new_client(argv[1], atoi(argv[2]));

	start_ui(cli);

	return 0;
}
