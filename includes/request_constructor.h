#ifndef __req_constr
#define __req_constr
#include "server.h"

ClientRequest* movie_add(char* buf);
ClientRequest* movie_delete(char* buf);
ClientRequest* screening_add(char* buf);
ClientRequest* screening_delete(char* buf);
ClientRequest* make_reservation(char* buf);
ClientRequest* movie_info(char* buf);
ClientRequest* movie_screenings(char* buf);
ClientRequest* seating_info(char* buf);
ClientRequest* movie_list(char* buf);
ClientRequest* reservation_list(char* buf);

#endif