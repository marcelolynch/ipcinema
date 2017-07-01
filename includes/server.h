#ifndef __srv_def
#define __srv_def

typedef enum { 
	REQ_MOVIE_ADD,
	REQ_MOVIE_DELETE,
	REQ_SCREENING_ADD,
	REQ_SCREENING_DELETE,
	REQ_SCREENING_INFO,
	REQ_MOVIE_INFO,
	REQ_MOVIE_SCREENINGS,
	REQ_MAKE_RESERVATION, 
	REQ_SEATING_INFO,
	REQ_MOVIE_LIST,
	REQ_RESERVATION_LIST,
	REQ_CANCELLED_LIST,
	REQ_CANCEL_RESERVATION
} requestType;


typedef enum{
	INVALID_REQUEST
} error_t;

typedef struct requestCDT {
	requestType type;
	void * data;
} ClientRequest;


#define MOVIE_NAME_LEN 127
#define CLIENT_NAME_LEN 63
#define DESCRIPTION_LEN 255
#define MAX_ID_LEN		32

typedef struct movieinfo{
	char name[MOVIE_NAME_LEN + 1];
	char description[DESCRIPTION_LEN + 1];
} MovieInfo;

typedef struct screeningid{
	char movie[MOVIE_NAME_LEN + 1];
	char id[MAX_ID_LEN];
	char day;
	char month;
	char slot;
	char sala;
} ScreeningInfo;


typedef struct tckt{
	ScreeningInfo screening;
	int seat;
} Ticket;


typedef struct reservationinfo{
	char seat;
	int screening_id;
	char client[CLIENT_NAME_LEN + 1];
} ReservationInfo;

struct resRequestCDT{ 
	char auditorium;
	char seat;
};


typedef struct resRequestCDT* ReservationRequest;


struct auditQueryCDT{ 
	int auditorium;
};

typedef struct auditQueryCDT* AuditoriumInfo;

#endif