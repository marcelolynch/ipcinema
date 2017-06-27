#include "errors.h"
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


int main(int argc, char*argv[]){

	//Para manejar yo lo errores de broken pipes
	signal(SIGPIPE, SIG_IGN);

  
  	if (argc != 3) fatal("Usage: client server-name server-port-number");

	ClientInstance cli = new_client(argv[1], atoi(argv[2]));

	char buf[1024];
	while(1){
		memset(buf, 0, 1024); 
		

		MovieInfo movie;

		scanf("%s", buf);
		while(getchar() != '\n');

		strcpy(movie.name, buf);
		strcpy(movie.description, "Generic description");
		
		add_movie(cli, &movie);
		memset(buf, 0, 1024); 
	}

}
