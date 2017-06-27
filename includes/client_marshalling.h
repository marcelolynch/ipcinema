typedef struct clientInstanceCDT* ClientInstance; 

#define MOVIE_NAME_LEN 127
#define DESCRIPTION_LEN 255
#define HALL_ROWS 8
#define HALL_COLS 8


typedef struct movieinfo{
	char name[MOVIE_NAME_LEN + 1];
	char description[DESCRIPTION_LEN + 1];
} MovieInfo;

typedef struct screeninginfo{
	char movie[MOVIE_NAME_LEN + 1];
	int day;
	int slot;
} ScreeningInfo;

typedef struct screenings{
	ScreeningInfo * list;
	int length;
} ScreeningsList;

typedef struct movies{
	MovieInfo * list;
	int length;
} MoviesList;


MoviesList getMovies();

ScreeningsList getScreenings(MovieInfo movie);

char * getHall(ScreeningInfo screening);


ClientInstance new_client(char* srv, int portno);

void add_movie(ClientInstance instance, MovieInfo* movie);

void delete_movie(ClientInstance instance, char* movie_name);

void add_screening(ClientInstance instance, ScreeningInfo* screening);

void delete_screening(ClientInstance instance, ScreeningInfo* screening);