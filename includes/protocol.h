#ifndef __proto_h
#define __proto_h

#define OK							20
#define ERROR						30
#define ERR_UNKNOWN   				31
#define ERR_NO_SUCH_DATA			32
#define ERR_INVALID_PACKET  		33
#define ERR_CONSTRAINT_VIOLATION 	35

#define TRANSACTION_BEGIN	70
#define	TRANSACTION_END		71
#define TRANSACTION_NEXT	72
#define TRANSACTION_ITEM	73


#define MOVIE_ADD 			0
#define MOVIE_DELETE 		1
#define SCREENING_ADD 		2
#define SCREENING_DELETE	3
#define SCREENING_INFO 		4
#define MAKE_RESERVATION	6
#define MOVIE_SCREENINGS	7
#define SEATING_INFO		8
#define MOVIE_LIST			9
#define RESERVATION_LIST	10
#define CANCEL_RESERVATION	11


#endif