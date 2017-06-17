#include "server.h"
#include "sockets.h"
#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string.h>
ClientRequest * wait_request(Connection con){
	char buf[BUF_SIZE] = {0};

	if(!receive_message(con, buf)){
		fatal("Error in receive");
	}

	printf("received: %s\n", buf);
	ClientRequest * req = malloc(sizeof(*req));
	if(strncmp(buf, "reserve", strlen("reserve")) == 0){
		req->type = RESERVATION;
		req->data = malloc(sizeof(*req->data));
		ReservationRequest r = req->data;
		int code = atoi(buf+strlen("reserve")+1);
		r->auditorium = code/100;
		r->seat = code%100;

	} else if(strncmp(buf, "info", strlen("info")) == 0){

		req->type = SEAT_INFO;
		req->data = malloc(sizeof(*req->data));

		AuditoriumInfo r = req->data;
		int code = atoi(buf+strlen("info")+1);
		r->auditorium = code;
	} else{
		return NULL;
	}

	return req;

}