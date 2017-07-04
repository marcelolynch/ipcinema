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


typedef struct screeningid{
	char movie[MOVIE_NAME_LEN + 1];
	char id[30];
	char day;
	char month;
	char slot;
	char sala;
} ScreeningInfo;



typedef struct reservationinfo{
	char seat;
	char screening_id[ID_LEN];
	char client[CLIENT_NAME_LEN + 1];
} ReservationInfo;



typedef struct tckt{
	ScreeningInfo screening;
	int seat;
} Ticket;


typedef enum{
	SRV_OK,
	PROTOCOL_ERROR,
	DB_VIOLATION,
	NO_SUCH_DATA,
	UNKNOWN
} response_t;

ListADT get_movies(ClientInstance instance);

ListADT get_screenings(ClientInstance instance, MovieInfo* movie);

/** El ultimo parametro indica si se estan pidiendo los cancelados (1 = cancelados, 0 = normal)*/
ListADT get_tickets(ClientInstance instance, char* client, int req_cancelled);

char * get_hall(ClientInstance instance, char* screening_id);

ClientInstance new_client(char* srv, int portno);

response_t add_movie(ClientInstance instance, MovieInfo* movie);

response_t delete_movie(ClientInstance instance, char* movie_name);

response_t add_screening(ClientInstance instance, ScreeningInfo* screening);

response_t delete_screening(ClientInstance instance, ScreeningInfo* screening);

response_t make_reservation(ClientInstance instance, ReservationInfo* res);

response_t cancel_reservation(ClientInstance instance, ReservationInfo* res);

#endif