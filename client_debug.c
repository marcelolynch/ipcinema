#include "errors.h"
#include "client_marshalling.h"
#include "utilities.h"

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>



int main(int argc, char*argv[]){
  
  	if (argc != 3){
	  	printf("Usage: client server-name server-port-number\n");
	  	exit(1);
  	}	

	srand(time(NULL));
	//Para manejar yo lo errores de broken pipes
	signal(SIGPIPE, SIG_IGN);


  	ClientInstance cli = new_client(argv[1], atoi(argv[2]));

	char buf[1024];
	while(1){
		memset(buf, 0, 1024); 
		
		char name[100];
		MovieInfo movie;
		ScreeningInfo screening;
		ReservationInfo res;

		int c = getchar();
		int a, b,e,d;
		int i;

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
				scanf("%s %d %d %d %d", screening.movie, &a, &b, &d, &e);
				screening.day = a;
				screening.month = b;
				screening.slot = d;
				screening.sala = e;
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
				ListADT screenings = get_screenings(cli, &movie);
				ListIteratorADT iter = get_iterator(screenings);
				while(iter_has_next(iter)){
					ScreeningInfo s;
					iter_get_next(iter, &s);
					printf("Screening id %s: %d/%d slot %d sala %d\n", s.id, s.day, s.month, s.slot, s.sala);
				}
				fflush(stdout);
				destroy_iterator(iter);
				destroy_list(screenings);

				break;
			}
			case 't':
			{
				scanf("%s", name);
				char* seats = get_hall(cli, name);
				for(i= 0 ; i < 64 ; i++){
					if(seats[i]){
						printf("Ocupado: %d\n", i);
					}
				}
				free(seats);
				break;
			}
			case 'p':
			{
				ListADT movies= get_movies(cli);
				ListIteratorADT iter = get_iterator(movies);

				while(iter_has_next(iter)){
					MovieInfo m;
					iter_get_next(iter, &m);
					printf("%s : %s\n", m.name, m.description);
				}
				
				destroy_list(movies);
				destroy_iterator(iter);
				break;
			}
			case 'x':
			{
				scanf("%s %s %d", res.client, res.screening_id, &b);
				
				res.seat = b;
				while(getchar() != '\n');
				cancel_reservation(cli, &res);
				break;

			}
			case 'z':
			{
				scanf("%s %d", name, &a);
				ListADT tickets = get_tickets(cli, name, a);
				ListIteratorADT iter = get_iterator(tickets);

				while(iter_has_next(iter)){
					Ticket t;
					iter_get_next(iter, &t);
					printf("Screening id %s, Date %d/%d, slot %d, hall %d: %s, asiento %d\n", t.screening.id, t.screening.day, t.screening.month, t.screening.slot, t.screening.sala, t.screening.movie, t.seat);
				}
				
				destroy_list(tickets);
				destroy_iterator(iter);

				break;
			}
			default: break;

		}
	}

	return 0;
}
