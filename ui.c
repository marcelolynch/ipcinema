#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client_marshalling.h"
#include "utilities.h"

#define ADMIN 1
#define CLIENT 2
#define EXIT_MAIN 3

#define MOVIES 10

#define ADD_MOVIE 1
#define ADD_SCREENING 2
#define DEL_MOVIE 3
#define DEL_SCREENING 4
#define SHOW_MOVIES 5
#define SHOW_SCREENING 6
#define EXIT_ADMIN 7

#define MAX_SLOTS 6
#define MAX_HALL 10
#define RESERVE_MOVIE 1
#define VIEW_RESERVATIONS 2
#define CANCEL_RESERVATION 3
#define EXIT 4

int getClientType();
void startClient();
void startAdministrator();
void getString(char q[],char name [], size_t max_len);


int start_ui(ClientInstance instance){
while(1){
	printf("************************************\n");
	printf("**                                **\n");
	printf("**      Welcome to ipCinemas      **\n");
	printf("**                                **\n");
	printf("************************************\n\n\n");
	int ans= getClientType();
	//printf("client type %d\n",ans);
	if(ans== CLIENT){
		printf("----- Starting client ----- \n");
		char name[CLIENT_NAME_LEN+1];
		getString("What's your name?: ",name, CLIENT_NAME_LEN);
		startClient(instance, name);
	} else if (ans == ADMIN){
		printf("----- Starting Administrator ----- \n");
		startAdministrator(instance);
	}else if(ans == EXIT_MAIN){
		return 0;
	}
	}
	return 0;
}

void clearBuffer(){
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

int getClientType(){
	while(1){
		printf("Client options: \n");
		printf("1 - Administrator\n");
		printf("2 - Client\n");
		printf("3 - Exit\n");
		printf("Select a client option number: ");
		char ans[128];
		scanf("%s",ans);
		clearBuffer();
		if(!strcmp(ans,"1")){
			return ADMIN;
		}else if (!strcmp(ans,"2")){
			return CLIENT;
		}else if (!strcmp(ans,"3")){
			return EXIT_MAIN;
		}else{
			printf("Not a valid number\n\n");
		}
	}
}

int showDays(){
	while(1){
		printf("These days we are open\n");
		printf("1 - Monday\n");
		printf("2 - Tuesday\n");
		printf("3 - Wednesady\n");
		printf("4 - Thursday\n");
		printf("5 - Firday\n");
		printf("6 - Saturday\n");
		printf("7 - Sunday\n");
		printf("Select a day for seeing movies of that day: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=7){
			clearBuffer();
			return ans;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}

char * getHour(int slot){
	char * ans;
	switch(slot){
		case 1: 
			ans="00:30";
			break;
		case 2: 
			ans="10:30";
			break;
		case 3: 
			ans="13:30";
			break;
		case 4: 
			ans="16:30";
			break;
		case 5: 
			ans="19:30";
			break;
		case 6: 
			ans="21:30";
			break;
		}
		return ans;
	}

void showMoviesOnly(ListADT movies){
	int i;
	ListIteratorADT iter = get_iterator(movies);
	MovieInfo movie;

	for(i=0; iter_has_next(iter); i++){
		iter_get_next(iter, &movie);
		printf("%d - %s\n",i+1, movie.name);
	}

	destroy_iterator(iter);
}


void showScreeningsOnly(ListADT screenings){
	ListIteratorADT iter = get_iterator(screenings);
	int i = 0;
	while(iter_has_next(iter)){
		ScreeningInfo s;
		iter_get_next(iter, &s); 
		printf("%d -%d/%d | Time: %s | Hall: %d\n",i+1, s.day, s.month, getHour(s.slot), s.sala);
		i++;
	}

	destroy_iterator(iter);
}

int showMovies(ListADT movies){
	if(list_length(movies) == 0){
		printf("There are no movies showing.\n");
		return -1;
	}
	while(1){
		printf("Here are the movies we're currently showing\n");
		showMoviesOnly(movies);
		printf("Insert number of the movie: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans <= list_length(movies)){
			clearBuffer();
			return ans-1;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}

}

int showSeats(char seats[]){
	while(1){
	int i,j,row,col;
	printf("\n\n These are the available seats: \n");
	printf("   1   2   3   4   5   6   7   8 \n");
	for (i = 0; i < HALL_ROWS; ++i){
		printf("%d ",i+1 );
		for (j = 0; j < HALL_COLS; ++j)
		{
			char c;
			if(seats[i*HALL_COLS+j]==1){
				c='X';
			}else{
				c=' ';
			}
			printf("[%c] ",c);
		}
		printf("\n");
	}
	printf("\nInsert a row and column for reserving: ");
	if(scanf("%d %d",&row,&col)==2 && row>0 && row<=HALL_ROWS && col>0 && col<=HALL_COLS){
				col-=1;
				row-=1;
		if(seats[row*HALL_COLS+col]==0){
			clearBuffer();
			return row*HALL_COLS+col;
		}else{
			printf("Seat is already taken\n");
		}
	}else{
		printf("Not a valid. Enter row col\n");
	}

	
	clearBuffer();
}


}

int showAdminOptions(){
	while(1){
		printf("\n\nWhat would you like to do?\n");
		printf("1 - Add movie\n");
		printf("2 - Add screening\n");
		printf("3 - Delete movie\n");
		printf("4 - Delete screening\n");
		printf("5 - Show movies\n");
		printf("6 - Show screenings\n");
		printf("7 - Exit\n");
		printf("Select an option: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=7){
			clearBuffer();
			return ans;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}

int showClientOptions(){
	while(1){
		printf("\n\nWhat would you like to do?\n");
		printf("1 - Reserve a movie\n");
		printf("2 - View reservations\n");
		printf("3 - Cancel reservation\n");
		printf("4 - Exit\n");
		printf("Select an option: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=4){
			clearBuffer();
			return ans;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}

void getString(char q[],char name [], size_t max_len){
	printf("%s",q);
	int c;
	int i=0;
	while((c=getchar())!='\n' && i < max_len){
		name[i]=c;
		i++;
	}
	if(c != '\n'){
		clearBuffer();
	}
	name[i]=0;
}

int getMovieInfo(char name [], char description []){
	getString("Insert movie name: ",name, MOVIE_NAME_LEN);
	getString("Insert movie description: ",description, DESCRIPTION_LEN);
	return 0;
}

int getNum(char string [],int max){
	while(1){
	printf("%s (1-%d): ",string,max);
	int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=max){
			clearBuffer();
			return ans;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}

int askYN(char question []){
	while(1){
		printf("%s (y/n): ",question);
		char ans=getchar();
		clearBuffer();
		if(ans=='n'){
			return 0;
		}else if(ans=='y'){
			return 1;
		}
		printf("Invalid command\n");
	}
}



int showScreenings(ListADT screenings){
	if(list_length(screenings) == 0){
		printf("There are no screenings for that movie.\n");
		return -1;
	}
	while(1){

	showScreeningsOnly(screenings);

	printf("Select a screening: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<= list_length(screenings)){
			clearBuffer();
			return ans-1;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}


int startReservation(ClientInstance instance, char name[]){
		// get movies from db
		ReservationInfo res;
		ListADT movies= get_movies(instance);
		int chosenM = showMovies(movies);
		
		if(chosenM == -1){
			destroy_list(movies);
			return -1;
		}

		MovieInfo chosen_movie;
		get_from_list(movies, chosenM, &chosen_movie);
		
		printf("Chosen movie: %d: %s\n", chosenM, chosen_movie.name);

		ListADT screenings = get_screenings(instance, &chosen_movie);
		// get days for movie
		int chosenSc = showScreenings(screenings);

		if(chosenSc == -1){
			destroy_list(movies);
			destroy_list(screenings);
			return -1;
		}

		ScreeningInfo chosen_screening;
		get_from_list(screenings, chosenSc, &chosen_screening);
	
		// get hall from db
		char * seats= get_hall(instance, chosen_screening.id);
		//int seat;
		char chosenSe = showSeats(seats);

		//printf("Trying to reserve row: %d col:%d\n",row,col );
		printf("Movie: %s\n",chosen_movie.name);
		printf("Day: %d Slot: %d Sala:%d\n", chosen_screening.day,chosen_screening.slot, chosen_screening.sala);
		printf("Row: %d Col: %d\n",(chosenSe/HALL_COLS)+1,(chosenSe%HALL_COLS)+1 );
		if(askYN("Do you want to make this reservation?")){
			printf("Trying to reserve... \n");
			res.seat=chosenSe;
			strcpy(res.screening_id, chosen_screening.id);
			strcpy(res.client,name);
			response_t r = make_reservation(instance, &res);
			if(r != SRV_OK){
				if(r == DB_VIOLATION){
					printf("It seems that the seat you chose became unavailable while you were pondering. Try again!\n");
				} else if(r == NO_SUCH_DATA){
					printf("It would seem that this screening isn't available anymore. Sorry!\n");
				} else { 
					printf("An error ocurred. Try again later\n");
				}
			}
		}else{
			printf("The reservation was cancelled.\n");
		}

		destroy_list(movies);
		destroy_list(screenings);
		free(seats);
		return 0;
}

void viewReservations(ListADT list){
	if(list_length(list)==0){
		printf("      There are no reservations.\n\n");
		return ;
	}
	int i;
	ListIteratorADT iter = get_iterator(list);
	Ticket ticket;

	for(i=0; iter_has_next(iter); i++){
		iter_get_next(iter, &ticket);
		ScreeningInfo s = ticket.screening;
		printf("%d - %s|| %d/%d | Time: %s | Hall: %d | Row: %d Col: %d \n",i+1, ticket.screening.movie, s.day, s.month, getHour(s.slot), s.sala,(ticket.seat/HALL_COLS)+1,(ticket.seat%HALL_COLS)+1);
	}
	printf("\n");

	destroy_iterator(iter);

}

int cancelReservation(ListADT list){
	if(list_length(list)==0){
		printf("There are no reservations.\n");
		return 0;
	}
	while(1){
		printf("These are your reservations: \n");
		viewReservations(list);
		printf("\nInsert number of reservation you want to cancel: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=list_length(list)){
			clearBuffer();
			return ans-1;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}

void startClient(ClientInstance instance, char name []){
	while(1){
		printf("\n\nWelcome, %s!\n",name );
		int option =showClientOptions();
		if(option == RESERVE_MOVIE){

			startReservation(instance,name);

		}else if(option == VIEW_RESERVATIONS){

			ListADT reserv=get_tickets(instance, name, 0);
			printf("These are your reservations: \n");
			viewReservations(reserv);

			ListADT cancelled=get_tickets(instance, name, 1);
			printf("These are your cancelled reservations: \n");
			viewReservations(cancelled);

		}else if (option == CANCEL_RESERVATION){
			ListADT reserv=get_tickets(instance, name, 0);
			int chosen =cancelReservation(reserv);
			Ticket ticket;
			get_from_list(reserv,chosen,&ticket);
			printf("Trying to cancel...\n");
			ReservationInfo res;
			res.seat=ticket.seat;
			strcpy(res.screening_id,ticket.screening.id);
			strcpy(res.client,name);
			cancel_reservation(instance, &res);
		}else if (option == EXIT){
			return;
		}
	}
}

void startAdministrator(ClientInstance instance){
	while(1){
	int option =showAdminOptions();
	if(option == ADD_MOVIE){
		MovieInfo movie;
		getMovieInfo(movie.name,movie.description);
		
		printf("Name: %s\n",movie.name );
		printf("Description: %s\n",movie.description );
		
		
		if(askYN("Do you want add this movie?")){
			printf("Adding movie...\n");
			response_t r = add_movie(instance,&movie);
			if(r != SRV_OK){
				if(r == DB_VIOLATION){
					printf("That movie was already added!\n");
				} else {
					printf("An error ocurred. Try again later\n");
				}
			}
		}else{
			printf("Operation cancelled\n");
		}
	}else if (option == ADD_SCREENING){
		ScreeningInfo screening;
		ListADT movies= get_movies(instance);

		int chosenM =showMovies(movies);
		MovieInfo chosen_movie;
		get_from_list(movies, chosenM, &chosen_movie);
		strcpy(screening.movie, chosen_movie.name);
		
		screening.month=getNum("Insert the month:",12);
		screening.day = getNum("Insert the day of the month:",31);
		screening.slot= getNum("Insert the slot:",MAX_SLOTS);
		screening.sala= getNum("Insert the hall:",MAX_HALL);
		printf("Name: %s\n",screening.movie);
		printf("Day: %d Month: %d Slot: %d Hall:%d\n",screening.day,screening.month,screening.slot,screening.sala);
		if(askYN("Do you want add this screening?")){
			printf("Adding screening ...\n");
			response_t r = add_screening(instance,&screening);
		
			if(r != SRV_OK){
				if(r == DB_VIOLATION){
					printf("That slot is already occupied!\n");
				} else if (r == NO_SUCH_DATA){
					printf("It seems that the movie for which the screening was selected was deleted.\n");
				} else {
					printf("An error ocurred. Try again later\n");
				}
			}

		}else{
			printf("Operation cancelled\n");
		}
		
	}else if(option == DEL_MOVIE){
		ListADT movies = get_movies(instance);
		int chosenM =showMovies(movies);
		
		MovieInfo chosen_movie;
		get_from_list(movies, chosenM, &chosen_movie);
		printf("Name: %s\n", chosen_movie.name);
		if(askYN("Do you want delete this movie?")){
			printf("Deleting movie ...\n");
			delete_movie(instance, chosen_movie.name);
		}else{
			printf("Operation cancelled\n");
		}
	}else if(option == DEL_SCREENING){
		ListADT movies = get_movies(instance);
		int chosenM = showMovies(movies);
		
		MovieInfo chosen_movie;
		get_from_list(movies, chosenM, &chosen_movie);

		ListADT screenings = get_screenings(instance, &chosen_movie);

		int chosenSc = showScreenings(screenings);
		
		if(chosenSc > 0){
			ScreeningInfo chosen_screening;
			get_from_list(screenings, chosenSc, &chosen_screening);


			if(askYN("Do you want delete this screening?")){
				printf("Deleting screening ...\n");
				delete_screening(instance,&chosen_screening);
			}else{
				printf("Operation cancelled\n");
			}
	
		}
		
		destroy_list(screenings);
		destroy_list(movies);

	}else if(option == EXIT_ADMIN){

		return;

	}else if(option == SHOW_MOVIES){

		ListADT movies = get_movies(instance);
		showMoviesOnly(movies);
		destroy_list(movies);

	} else if(option == SHOW_SCREENING){

		ListADT movies = get_movies(instance);
		int chosenM = showMovies(movies);
		MovieInfo chosen_movie;
		get_from_list(movies, chosenM, &chosen_movie);
		ListADT screenings = get_screenings(instance, &chosen_movie);
		
		if(list_length(screenings) == 0){
			printf("There are no screenigns for that movie.\n");

		} else {
			showScreeningsOnly(screenings);
		}

		destroy_list(movies);
		destroy_list(screenings);
	}
}
}

