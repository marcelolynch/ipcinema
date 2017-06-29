#include <stdio.h>
#include <stdlib.h>
#include "client_marshalling.h"
#include "errors.h"
#include "string.h"

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

#define MAX_SLOTS 10
#define MAX_HALL 10
#define RESERVE_MOVIE 1
#define VIEW_RESERVATIONS 2
#define CANCEL_RESERVATION 3
#define EXIT 4

int getClientType();
int startClient();
int startAdministrator();



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
		startClient(instance);
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
	char c;
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

int showMovies(MoviesList movies){
	if(movies.length==0){
		printf("There are no movies showing.\n");
		return -1;
	}
	while(1){
		int i;
		printf("\n\nThese are the movies showing:\n");
		for(i=0; i<movies.length; i++){
			printf("%d - %s\n",i+1,movies.list[i].name );
		}
	
		printf("\nInsert number of the movie: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=movies.length){
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
	printf("\n These are the available seats: \n");
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
		printf("What would you like to do?\n");
		printf("1 - Add movie\n");
		printf("2 - Add screening\n");
		printf("3 - Delte movie\n");
		printf("4 - Delte screening\n");
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
		printf("What would you like to do?\n");
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

void getString(char q[],char name []){
	printf("%s",q);
	char c;
	int i=0;
	while((c=getchar())!='\n'){
		name[i]=c;
		i++;
	}
	name[i]=0;
	//scanf("%s",name);
	//clearBuffer();
}

int getMovieInfo(char name [], char description []){
	getString("Insert movie name: ",name);
	getString("Insert movie description: ",description);
	return 0;
}
int getSlot(){
	while(1){
	printf("Insert a slot (1-%d): ",MAX_SLOTS);
	int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=MAX_SLOTS){
			clearBuffer();
			return ans;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}

int getSala(){
	while(1){
	printf("Insert a hall (1-%d): ",MAX_HALL);
	int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=MAX_SLOTS){
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



int showScreenings(ScreeningsList screenings){
	if(screenings.length==0){
		printf("There are no movies showing.\n");
		return -1;
	}
	while(1){
	int i;
	printf("These are the screenings: \n");
	for(i =0; i<screenings.length ; i++){
		printf("%d -Day: %d Slot:%d Sala:%d\n",i+1,screenings.list[i].day,screenings.list[i].slot,screenings.list[i].sala );
	}
	printf("Select a screening: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=screenings.length){
			clearBuffer();
			return ans-1;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}
int startReservation(ClientInstance instance){
	//int seats[HALL_ROWS][HALL_COLS]={{0,0,0,0,1,0,1,0}};
	//char movies [MOVIES][128]={"Harry Potter 1","Harry Potter 2","Harry Potter 3","Harry Potter 4","Harry Potter 5","Harry Potter 6","Harry Potter 7: Parte 1","Harry Potter 7: Parte 2","Animales Fantasitcos y Donde Encontrarlos","Animales Fantasitcos y Donde Encontrarlos 2"};
	
	while(1){
		// get movies from db
		ReservationInfo res;
		MoviesList * movies= get_movies(instance);
		int chosenM =showMovies(*movies);
		if(chosenM == -1){
			return -1;
		}
		printf("Chosen movie %d\n", chosenM );
		ScreeningsList * screenings=get_screenings(instance, &(movies->list[chosenM]));
		// get days for movie
		int chosenSc =showScreenings(*screenings);
		if(chosenSc==-1){
			return -1;
		}

		// get hall from db
		char * seats= get_hall(instance, screenings->list[chosenSc].id);
		//int seat;
		char chosenSe=showSeats(seats);
		//printf("Trying to reserve row: %d col:%d\n",row,col );
		printf("Movie: %s\n",movies->list[chosenM].name);
		printf("Day:%d Slot:%d Sala:%d\n",screenings->list[chosenSc].day,screenings->list[chosenSc].slot,screenings->list[chosenSc].sala );
		printf("Row: %d Col:%d\n",(chosenSe/HALL_COLS)+1,(chosenSe%HALL_COLS)+1 );
		if(askYN("Do you want to make this reservation?")){
			printf("Trying to reserve... \n");
			res.seat=chosenSe;
			strcpy(res.screening_id,screenings->list[chosenSc].id);
			strcpy(res.client,"Juancito");
			make_reservation(instance, &res);
			return 0;
		}else{
			printf("The reservation was cancelled.\n");
		}
	}
}

void viewReservations(char reserv[][128], int len){
	
	if(len==0){
		printf("There are no reservations.\n");
		return ;
	}
	printf("This are your reservations:\n");
	int i;
	for(i=0; i<len;i++){
		printf("%d - %s\n",i+1,reserv[i] );
	}

}

int cancelReseravtion(char reserv[][128], int len){
	if(len==0){
		printf("There are no reservations.\n");
		return 0;
	}
	while(1){
		viewReservations(reserv,len);
		printf("\nInsert number of reservation you want to cancel: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=len){
			clearBuffer();
			return ans-1;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}

int startClient(ClientInstance instance){
	while(1){
		int option =showClientOptions();
		if(option == RESERVE_MOVIE){
			startReservation(instance);
		}else if(option == VIEW_RESERVATIONS){
			char reserv [2][128]={"HArry postre","maluma"};
			viewReservations(reserv,2);
		}else if (option == CANCEL_RESERVATION){
			char reserv [2][128]={"HArry postre","maluma"};
			cancelReseravtion(reserv,2);
		}else if (option == EXIT){
			return 0;
		}
	}
}

int startAdministrator(ClientInstance instance){
	while(1){
	int option =showAdminOptions();
	if(option == ADD_MOVIE){
		MovieInfo movie;
		getMovieInfo(movie.name,movie.description);
		
		printf("Name: %s\n",movie.name );
		printf("Description: %s\n",movie.description );
		
		
		if(askYN("Do you want add this movie?")){
			printf("Adding movie...\n");
			add_movie(instance,&movie);
		}else{
			printf("Operation cancelled\n");
		}
	}else if (option == ADD_SCREENING){
		ScreeningInfo screening;
		MoviesList * movies= get_movies(instance);
		int chosenM =showMovies(*movies);
		strcpy(screening.movie, movies->list[chosenM].name);
		screening.day = showDays();
		screening.slot= getSlot();
		screening.sala= getSlot();
		printf("Name: %s\n",screening.movie );
		printf("Day: %d Slot: %d Sala:%d\n",screening.day,screening.slot,screening.sala);
		if(askYN("Do you want add this screening?")){
			printf("Adding screening ...\n");
			add_screening(instance,&screening);
		}else{
			printf("Operation cancelled\n");
		}
		
	}else if(option == DEL_MOVIE){
		MoviesList * movies= get_movies(instance);
		int chosenM =showMovies(*movies);
		
		printf("Name: %s\n",movies->list[chosenM].name );
		if(askYN("Do you want delete this movie?")){
			printf("Deleting movie ...\n");
			delete_movie(instance,movies->list[chosenM].name);
		}else{
			printf("Operation cancelled\n");
		}
	}else if(option == DEL_SCREENING){
		ScreeningInfo screening;
		getString("Insert movie name: ",screening.movie);
		screening.day = showDays();
		screening.slot= getSlot();
		screening.sala= getSlot();
		printf("Name: %s\n",screening.movie );
		printf("Day: %d Slot: %d Sala:%d\n",screening.day,screening.slot,screening.sala);
		if(askYN("Do you want add this screening?")){
			printf("Deleting screening ...\n");
			delete_screening(instance,&screening);
		}else{
			printf("Operation cancelled\n");
		}
		
	}else if(option == EXIT_ADMIN){
		return 0;
	}else if(option == SHOW_MOVIES){
		MoviesList * movies= get_movies(instance);
		showMovies(*movies);
	}else if(option == SHOW_SCREENING){
		MoviesList * movies= get_movies(instance);
		int chosenM= showMovies(*movies);
		ScreeningsList * screenings=get_screenings(instance, &(movies->list[chosenM]));
		if(screenings->length==0){
			printf("There are no movies showing.\n");
		}else{
			int i;
			printf("These are the screenings: \n");
			for(i =0; i<screenings->length ; i++){
				printf("%d -Day: %d Slot:%d Sala:%d\n",i+1,screenings->list[i].day,screenings->list[i].slot,screenings->list[i].sala );
			}
		}
	}
}
}

