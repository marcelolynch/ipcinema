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
		
		char name[100];
		MovieInfo movie;
		ScreeningInfo screening;
		ReservationInfo res;

		int c = getchar();
		int a, b;

		while(getchar() != '\n');
		switch(c){
			case 'a':
			{
				scanf("%s %s", movie.name, movie.description);
				while(getchar() != '\n');
				add_movie(cli, &movie);
				break;
			}
			case 'd':
			{
				scanf("%s", name);
				while(getchar() != '\n');
				delete_movie(cli, name);
				break;
			}
			case 's':
			{
				scanf("%s %d %d", screening.movie, &a, &b);
				screening.day = a;
				screening.slot = b;
				while(getchar() != '\n');
				add_screening(cli, &screening);
				break;
			}
			case 'r':
			{
				scanf("%s %s %d", res.client, res.screening_id, &b);
				
				res.seat = b;
				while(getchar() != '\n');
				make_reservation(cli, &res);
				break;
			}
			case 'g':
			{
				scanf("%s", movie.name);
				while(getchar() != '\n');
				ScreeningsList screenings = get_screenings(cli, &movie);
				int i;
				for(i = 0; i < screenings.length ; i++){
					printf("Screening id %s: Day %d slot %d sala %d\n", screenings.list[i].id,screenings.list[i].day,screenings.list[i].slot, screenings.list[i].sala);
				}
				fflush(stdout);
				break;
;

			}
		}
	}

	return 0;
}
