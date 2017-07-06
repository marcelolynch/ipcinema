#include "server.h"
#include "sockets.h"
#include "server_marshalling.h"
#include "server_logging.h"
#include "protocol.h"
#include "request_builder.h"
#include "utilities.h"

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string.h>

/* 

 Abstraigo a la capa superior de las dependencias de comunicación y del protocolo.
 En definitiva serverinstanceCDT wrappea el listener socket y
 clientsessionCDT el socket de cada cliente. Por como se utilizan ahora,
 podrían devolverse Address o Connection directamente pero este diseño es más extensible.

 En server_marshalling.h se definen los punteros a estas estructuras para funcionar como ADT .

 Esta capa conoce el protocolo definido entre servidor y cliente y se encarga de la serialziación
 de las respuestas y la deserialización de las requests.

*/

struct serverinstanceCDT {
	Address addr;
};


struct clientsessionCDT {
	Connection con;
	char status;
};

/* Abre el socket en el que se escucharán conexiones 
	y hace el bind & listen */
ServerInstance server_init(int port){
	ServerInstance session = failfast_malloc(sizeof(struct serverinstanceCDT));

	session->addr = open_socket(NULL, port);

	if(session->addr == NULL){
		srv_log("[ERROR] Socket opening failed");
		srv_log("Exiting on error");
		free(session);
		exit(3);
	}

	if(socket_bind(session->addr) < 0){
		srv_log("[ERROR] Socket bind failed. Can't continue");
		srv_log("Exiting on error");
		free(session);
		exit(4);
	}

	return session;
} 

 
ClientSession wait_client(ServerInstance srv){
	Connection c = accept_connection(srv->addr);
	
	if(c == NULL){
		srv_log("[ERROR] Error establishing connection with client");
		return NULL;
	}

	ClientSession cli = failfast_malloc(sizeof(struct clientsessionCDT));
	cli->con = c;

	return cli;
}


void end_session(ClientSession session){
	destroy_connection(session->con);
	free(session);
}


/* Espero una request del cliente y llamo al constructor de ClientRequest que corresponda
   para finalmente devolver ese objeto. Ver request_builder.c y server.h */
ClientRequest * wait_request(ClientSession cli){
	//El buffer termina con un cero para protegerme
	//Cuando hago strlen de no sobrepasarme
	char buf[PACKET_LENGTH + 1] = {0};

	int rd = receive_message(cli->con, buf, PACKET_LENGTH);
	if(rd < 0){
		srv_log("[WARNING] Error in receive from client");
		return NULL;
	}
	if(rd == 0){
		srv_log("[CLIENT] Closed socket. Closing connection");
		destroy_connection(cli->con);
		free(cli);
		pthread_exit(NULL);
	}

	// El primer byte del paquete contiene el codigo de request
	switch(buf[0]){
		case MOVIE_ADD: 		return req_movie_add(buf);
		case MOVIE_DELETE: 		return req_movie_delete(buf);
		case SCREENING_ADD: 	return req_screening_add(buf);
		case SCREENING_DELETE: 	return req_screening_delete(buf);
		case MAKE_RESERVATION: 	return req_make_reservation(buf);
		case CANCEL_RESERVATION:return req_cancel_reservation(buf);
		case MOVIE_SCREENINGS:	return req_movie_screenings(buf);
		case SEATING_INFO:		return req_seating_info(buf);
		case MOVIE_LIST:		return req_movie_list(buf);
		case RESERVATION_LIST:	return req_reservation_list(buf);
		default:
			srv_log("[WARNING] Received unknown request. Ignoring...");
	}

	return NULL;

}

/** Las funciones que siguen (send_screenings, send_movies, send_tickets)
	Son análogas en su funcionamiento: inician una transacción
	de paquetes con el cliente y envían en cada paquete una unidad
	de lo requerido (screening, movie, tickets) en cada caso.

	Se le envía un paquete TRANSACTION_BEGIN al cliente, junto
	con la cantidad de datos máxima que se van a enviar. El cliente 
	debe estar esperandolo por la request que envió. Se esperan
	pedidos de TRANSACTION_NEXT hasta que no hay mas datos que envíar.
	En ese momento se envía TRANSACTION_END. Si se se reciba algún
	otro mensaje distinto de TRANSACTION_NEXT se envía un acknowledgement (OK) y se termina.

	La serializacion de cada dato pedido es evidente en cada función, y como siempre
	el byte 0 del paquete funciona como header e indica el dato que se está enviando
 */

int send_screenings(ClientSession session, ListADT screenings){
	char buf[PACKET_LENGTH] = {0};


	buf[0] = TRANSACTION_BEGIN;
	buf[1] = list_length(screenings);
	
	send_message(session->con, buf, PACKET_LENGTH);
	receive_message(session->con, buf, PACKET_LENGTH);

	ListIteratorADT iter = get_iterator(screenings);

	while(buf[0] == TRANSACTION_NEXT && iter_has_next(iter)){
		ScreeningInfo data;

		iter_get_next(iter, &data);

		buf[0] = TRANSACTION_ITEM;
		buf[1] = data.day;
		buf[2] = data.month;
		
		buf[3] = data.slot;
		buf[4] = data.sala;


		strcpy(&buf[5], data.id);

		send_message(session->con, buf, PACKET_LENGTH);
		receive_message(session->con, buf, PACKET_LENGTH);

	}
	
	if(buf[0] == TRANSACTION_NEXT){
		buf[0] = TRANSACTION_END;
		send_message(session->con, buf, PACKET_LENGTH);	
	} else {
		//El cliente cancelo la transacción, mando acknowledgement
		buf[0] = OK;
		send_message(session->con, buf, PACKET_LENGTH);
	}

	destroy_iterator(iter);
	return 1;
}


int send_movies(ClientSession session, ListADT movies){
	char buf[PACKET_LENGTH] = {0};
	
	char count = list_length(movies);

	buf[0] = TRANSACTION_BEGIN;
	buf[1] = count;
		
	send_message(session->con, buf, PACKET_LENGTH);
	receive_message(session->con, buf, PACKET_LENGTH);

	ListIteratorADT iter = get_iterator(movies);

	while(buf[0] == TRANSACTION_NEXT && iter_has_next(iter)){
		MovieInfo info;
		iter_get_next(iter, &info);
	
		buf[0] = TRANSACTION_ITEM;
		strcpy(&buf[1], info.name);
		strcpy(&buf[1+strlen(&buf[1])+1], info.description);

		send_message(session->con, buf, PACKET_LENGTH);
		receive_message(session->con, buf, PACKET_LENGTH);
	}
	
	if(buf[0] == TRANSACTION_NEXT){
		buf[0] = TRANSACTION_END;
		send_message(session->con, buf, PACKET_LENGTH);	
	} else {
		//El cliente cerro la conexion, mando acknowledgement
		buf[0] = OK;
		send_message(session->con, buf, PACKET_LENGTH);
	}

	destroy_iterator(iter);

	return 1;
}



int send_tickets(ClientSession session, ListADT tickets){
	char buf[PACKET_LENGTH] = {0};
	
	char count = list_length(tickets);

	buf[0] = TRANSACTION_BEGIN;
	buf[1] = count;
		
	send_message(session->con, buf, PACKET_LENGTH);
	receive_message(session->con, buf, PACKET_LENGTH);

	ListIteratorADT iter = get_iterator(tickets);

	while(buf[0] == TRANSACTION_NEXT && iter_has_next(iter)){
		Ticket ticket;
		iter_get_next(iter, &ticket);
	
		buf[0] = TRANSACTION_ITEM;
		buf[1] = ticket.seat;
		buf[2] = ticket.screening.day;
		buf[3] = ticket.screening.month;
		buf[4] = ticket.screening.slot;
		buf[5] = ticket.screening.sala;
		strcpy(&buf[6], ticket.screening.id);

		int len = strlen(&buf[6]);

		strcpy(&buf[6+len+1], ticket.screening.movie);


		send_message(session->con, buf, PACKET_LENGTH);
		receive_message(session->con, buf, PACKET_LENGTH);
	}
	
	if(buf[0] == TRANSACTION_NEXT){
		buf[0] = TRANSACTION_END;
		send_message(session->con, buf, PACKET_LENGTH);	
	} else {
		//El cliente cerro la conexion, mando acknowledgement
		buf[0] = OK;
		send_message(session->con, buf, PACKET_LENGTH);
	}

	destroy_iterator(iter);

	return 1;
}


// Envio información de los asientos. Size debe ser tal que no se
// exceda el tamaño del paquete (< 512 bytes).
int send_seats(ClientSession session, char* seats, int size){
	char buf[PACKET_LENGTH];
	buf[0] = OK;
	memcpy(&buf[1], seats, size);
	send_message(session->con, buf, PACKET_LENGTH);
	return 1;
}

static char get_error_code(error_t error){
	switch(error){
		case INVALID_REQUEST: 		return ERR_INVALID_PACKET;			break;
		case NO_SUCH_ELEMENT:		return ERR_NO_SUCH_DATA;
		case CONSTRAINT_VIOLATION: 	return ERR_CONSTRAINT_VIOLATION;	break;
		default: 					return ERR_UNKNOWN;					break;
	}
}

void client_send_error(ClientSession cli, error_t error){
	char buf[PACKET_LENGTH];
	buf[0] = ERROR;
	buf[1] = get_error_code(error);
	send_message(cli->con, buf, PACKET_LENGTH);
}


/* Enviar OK, mensaje de acknowledgement */
void client_send_ok(ClientSession cli){
	char buf[PACKET_LENGTH] = {0};
	buf[0] = OK;
	send_message(cli->con, buf, PACKET_LENGTH);
}