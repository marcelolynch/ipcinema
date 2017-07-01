#ifndef __cli_marsh_h
#define __cli_marsh_h

#include "utilities.h"

typedef struct clientInstanceCDT* ClientInstance; 

#define MOVIE_NAME_LEN 127
#define ID_LEN 20
#define DESCRIPTION_LEN 255
#define CLIENT_NAME_LEN 63

#define HALL_ROWS 8
#define HALL_COLS 8


typedef struct movieinfo{
	char name[MOVIE_NAME_LEN + 1];
	char description[DESCRIPTION_LEN + 1];
} MovieInfo;

typedef struct screeninginfo{
	char movie[MOVIE_NAME_LEN + 1];
	char day;
	char month;
	char slot;
	char sala;
} ScreeningInfo;


typedef struct screeningid{
	char id[30];
	char day;
	char month;
	char slot;
	char sala;
} ScreeningData;


typedef struct screenings{
	ScreeningData * list;
	int length;
} sl;

typedef struct movies{
	MovieInfo * list;
	int length;
} ml;

typedef struct reservationinfo{
	char seat;
	char screening_id[ID_LEN];
	char client[CLIENT_NAME_LEN + 1];
} ReservationInfo;

typedef struct tckt{
	ScreeningInfo screening;
	int seat;
} Ticket;


ListADT get_movies(ClientInstance instance);

ListADT get_screenings(ClientInstance instance, MovieInfo* movie);

char * get_hall(ClientInstance instance, char* screening_id);

ClientInstance new_client(char* srv, int portno);

void add_movie(ClientInstance instance, MovieInfo* movie);

void delete_movie(ClientInstance instance, char* movie_name);

void add_screening(ClientInstance instance, ScreeningInfo* screening);

void delete_screening(ClientInstance instance, ScreeningInfo* screening);

void make_reservation(ClientInstance instance, ReservationInfo* res);

#endif