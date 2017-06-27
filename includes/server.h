#ifndef __srv_def
#define __srv_def

typedef enum { 
	REQ_MOVIE_ADD,
	REQ_MOVIE_DELETE,
	REQ_SCREENING_ADD,
	REQ_SCREENING_DELETE,
	REQ_SCREENING_INFO,
	REQ_MOVIE_INFO,
	REQ_RESERVATION, 
	REQ_SEAT_INFO } requestType;

typedef struct requestCDT {
	requestType type;
	void * data;
}ClientRequest;


#define MOVIE_NAME_LEN 127
#define DESCRIPTION_LEN 255

typedef struct movieinfo{
	char name[MOVIE_NAME_LEN + 1];
	char description[DESCRIPTION_LEN + 1];
} MovieInfo;

typedef struct screeninginfo{
	char movie[MOVIE_NAME_LEN + 1];
	int day;
	int slot;
} ScreeningInfo;



struct resRequestCDT{ 
	int auditorium;
	int seat;
};

typedef struct resRequestCDT* ReservationRequest;


struct auditQueryCDT{ 
	int auditorium;
};

typedef struct auditQueryCDT* AuditoriumInfo;

#endif