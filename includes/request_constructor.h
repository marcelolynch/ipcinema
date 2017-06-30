#ifndef __req_constr
#define __req_constr
#include "server.h"

ClientRequest* req_movie_add(char* buf);
ClientRequest* req_movie_delete(char* buf);
ClientRequest* req_screening_add(char* buf);
ClientRequest* req_screening_delete(char* buf);
ClientRequest* req_make_reservation(char* buf);
ClientRequest* req_movie_info(char* buf);
ClientRequest* req_movie_screenings(char* buf);
ClientRequest* req_seating_info(char* buf);
ClientRequest* req_movie_list(char* buf);
ClientRequest* req_reservation_list(char* buf);

#endif