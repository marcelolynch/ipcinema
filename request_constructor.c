#include "request_constructor.h"
#include "server.h"
#include "sockets.h"
#include "utilities.h"

#include <string.h>


static int check_packet_bound(int index){
	if(index >= PACKET_LENGTH){
		return 0;
	}
	return 1;
}

ClientRequest* movie_add(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));

	char* name = &buf[1];

	int client_name_idx = 1 + strlen(name) + 1;

	if(!check_packet_bound(client_name_idx)){
		free(req);
		return NULL;
	}

	char* desc = &buf[client_name_idx];

	req->type = REQ_MOVIE_ADD;
	req->data = failfast_malloc(sizeof(MovieInfo));
	
	MovieInfo* moviedata = (MovieInfo*)req->data;
	safe_strncpy(moviedata->name, name, MOVIE_NAME_LEN);
	safe_strncpy(moviedata->description, desc, DESCRIPTION_LEN);

	return req;
}


ClientRequest* movie_delete(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));
	char* name = &buf[1];

	req->type = REQ_MOVIE_DELETE;
	req->data = failfast_malloc(strlen(name) + 1);
	strcpy((char*)req->data, name);

	return req;
}

ClientRequest* screening_add(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));
	req->type = REQ_SCREENING_ADD;
	req->data = failfast_malloc(sizeof(ScreeningInfo));
	ScreeningInfo* info = (ScreeningInfo*)req->data;

	info->day = buf[1];
	info->slot = buf[2];
	safe_strncpy(info->movie, &buf[3], MOVIE_NAME_LEN);

	return req;
}


ClientRequest* screening_delete(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));
	req->type = REQ_SCREENING_DELETE;
	req->data = failfast_malloc(sizeof(ScreeningInfo));
	ScreeningInfo* info = (ScreeningInfo*)req->data;

	info->day = buf[1];
	info->slot = buf[2];
	return req;
}


ClientRequest* make_reservation(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));
	req->type = REQ_MAKE_RESERVATION;
	req->data = failfast_malloc(sizeof(ReservationInfo));
	ReservationInfo* info = (ReservationInfo*)req->data;
	info->seat = buf[1];

	info->screening_id = atoi(&buf[2]);

	int client_name_idx = 2 + strlen(&buf[2] + 1);
	
	if(!check_packet_bound(client_name_idx)){
		free(req);
		return NULL;
	}

	safe_strncpy(info->client, &buf[client_name_idx], CLIENT_NAME_LEN);
	return req;
}


ClientRequest* movie_info(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));
	req->type = REQ_MOVIE_INFO;
	req->data = failfast_malloc(strlen(&buf[1] + 1));
	strcpy((char*)req->data, &buf[1]);	
	return req;
}


ClientRequest* movie_screenings(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));

	req->type = REQ_MOVIE_SCREENINGS;
	req->data = failfast_malloc(strlen(&buf[1] + 1));
	strcpy((char*)req->data, &buf[1]);	

	return req;
}


ClientRequest* seating_info(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));

	req->type = REQ_SEATING_INFO;
	req->data = failfast_malloc(strlen(&buf[1] + 1));
	strcpy((char*)req->data, &buf[1]);	
	
	return req;
}


ClientRequest* movie_list(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));
	req->type = REQ_MOVIE_LIST;
	return req;
}


ClientRequest* reservation_list(char* buf){
	ClientRequest * req = failfast_malloc(sizeof(*req));

	if(buf[1] == 0){
		req->type = REQ_RESERVATION_LIST;
	} else {
		req->type = REQ_CANCELLED_LIST;
	}
	req->data = failfast_malloc(strlen(&buf[2] + 1));

	return req;
}
