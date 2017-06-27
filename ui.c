#include <stdio.h>

#define ADMIN 1
#define CLIENT 2
#define HALL_ROWS 8
#define HALL_COLS 8
#define MOVIES 10
#define ADD_MOVIE 1
#define ADD_SCREENING 2
#define DEL_MOVIE 3
#define DEL_SCREENING 4
#define MOVIE_NAME_LEN 127
#define DESCRIPTION_LEN 255
#define MAX_SLOTS 10

int main(){
	

	printf("************************************\n");
	printf("**                                **\n");
	printf("**      Welcome to ipCinemas      **\n");
	printf("**                                **\n");
	printf("************************************\n\n\n");
	int ans= getClientType();
	//printf("client type %d\n",ans);
	if(ans== CLIENT){
		printf("----- Starting client ----- \n");
		startClient();
	}else if (ans == ADMIN){
		printf("----- Starting Administrator ----- \n");
		startAdministrator();
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
		printf("\nSelect a client option number: ");
		char ans[128];
		scanf("%s",ans);
		if(!strcmp(ans,"1")){
			return ADMIN;
		}else if (!strcmp(ans,"2")){
			return CLIENT;
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

int showMovies(char movies [][128], int length){
	while(1){
		int i;
		printf("\n\nThese are the movies showing:\n");
		for(i=0; i<length; i++){
			printf("%d - %s\n",i+1,movies[i] );
		}
	
		printf("\nInsert number of the movie you want to see: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=length){
			clearBuffer();
			return ans-1;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}

}

int showSeats(int seats[HALL_ROWS][HALL_COLS], int * row, int * col){
	while(1){
	int i,j;
	printf("\n These are the available seats: \n");
	printf("   1   2   3   4   5   6   7   8 \n");
	for (i = 0; i < HALL_ROWS; ++i){
		printf("%d ",i+1 );
		for (j = 0; j < HALL_COLS; ++j)
		{
			char c;
			if(seats[i][j]==1){
				c='X';
			}else{
				c=' ';
			}
			printf("[%c] ",c);
		}
		printf("\n");
	}
	printf("\nInsert a row and column for reserving: ");
	if(scanf("%d %d",row,col)==2 && *row>0 && *row<=HALL_ROWS && *col>0 && *col<=HALL_COLS){
				*col-=1;
				*row-=1;
		if(seats[*row][*col]==0){
			clearBuffer();
			return 1;
		}else{
			printf("Seat is already taken\n");
		}
	}else{
		printf("Not a valid. Enter row col\n");
	}

	
	clearBuffer();
}


}

int showOptions(){
	while(1){
		printf("What would you like to do?\n");
		printf("1 - Add movie\n");
		printf("2 - Add screening\n");
		printf("3 - Delte movie\n");
		printf("4 - Delte screening\n");
		printf("Select an option: ");
		int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=4){
			return ans;
		}
		printf("Not a valid option\n\n");
		while (getchar() != '\n');
	}
}

int getMovieName(char name []){
	printf("Insert movie name: ");
	scanf("%s",name);
	clearBuffer();
}

int getMovieInfo(char name [], char description []){
	getMovieName(name);
	printf("Insert movie description: ");
	scanf("%s",description);
	clearBuffer();
	printf("\n");
	return 0;
}
int getSlot(){
	while(1){
	printf("Insert a slot (1-%d): ",MAX_SLOTS);
	int ans;
		if(scanf("%d",&ans)>0 && ans>0 && ans<=MAX_SLOTS){
			clearBuffer();
			return ans-1;
		}
		printf("Not a valid option\n\n");
		clearBuffer();
	}
}
int askYN(char question []){
	while(1){
		printf("%s",question);
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

int startClient(){
	int seats[HALL_ROWS][HALL_COLS]={{0,0,0,0,1,0,1,0}};
	char movies [MOVIES][128]={"Harry Potter 1","Harry Potter 2","Harry Potter 3","Harry Potter 4","Harry Potter 5","Harry Potter 6","Harry Potter 7: Parte 1","Harry Potter 7: Parte 2","Animales Fantasitcos y Donde Encontrarlos","Animales Fantasitcos y Donde Encontrarlos 2"};
	
	while(1){
		// get movies from db
		int movie =showMovies(movies,MOVIES);
		
		// get days for movie
		int day =showDays();
		
		// get hall from db
		int row,col;
		showSeats(seats,&row,&col);
		printf("Trying to reserve row: %d col:%d\n",row,col );

		//try to resrve 
		if(!askYN("Do you want to make another reservation? (y/n): ")){
			return 0;
		}
	}
}

int startAdministrator(){
	while(1){
	int option =showOptions();
	if(option == ADD_MOVIE){
		char name [MOVIE_NAME_LEN];
		char description [DESCRIPTION_LEN];
		getMovieInfo(name,description);
		printf("Adding movie...\n");
	}else if (option == ADD_SCREENING){
		char name[MOVIE_NAME_LEN];
		getMovieName(name);
		int day = showDays();
		int slot= getSlot();
		printf("Adding screening ...\n");
	}else if(option == DEL_MOVIE){
		char name[MOVIE_NAME_LEN];
		getMovieName(name);
		printf("Deleting movie ...\n");
	}else if(option == DEL_SCREENING){
		char name[MOVIE_NAME_LEN];
		getMovieName(name);
		int day = showDays();
		int slot= getSlot();
		printf("Deleting screening ...\n");
	}
}
}

