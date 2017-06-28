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
	REQ_SEAT_INFO,
} requestType;

typedef struct requestCDT {
	requestType type;
	void * data;
} ClientRequest;


#define MOVIE_NAME_LEN 127
#define CLIENT_NAME_LEN 63
#define DESCRIPTION_LEN 255

typedef struct movieinfo{
	char name[MOVIE_NAME_LEN + 1];
	char description[DESCRIPTION_LEN + 1];
} MovieInfo;



typedef struct screeninginfo{
	char movie[MOVIE_NAME_LEN + 1];
	char day;
	char slot;
} ScreeningInfo;


typedef struct screeningid{
	char id[30];
	char day;
	char slot;
	char sala;
} ScreeningData;

typedef struct screeninglist{
	ScreeningData data;
	struct screeninglist * next;
} ScreeningDataList;	

typedef struct reservationinfo{
	char seat;
	char screening_id;
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