#ifndef __srv_def
#define __srv_def

typedef enum { RESERVATION, SEAT_INFO } requestType;

typedef struct requestCDT {
	requestType type;
	void * data;
}ClientRequest;


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