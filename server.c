#include "database.h"
#include "queries.h"
#include "server.h"
#include "server_logging.h"
#include "server_marshalling.h"
#include "utilities.h"
#include "synchronization.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>


static void * thread_work(void* data);
static void process_request(ClientSession session, ClientRequest* req);


static void add_movie(ClientSession session, MovieInfo* info);
static void delete_movie(ClientSession session, char* name);
static void add_screening(ClientSession session, ScreeningInfo* info);
static void delete_screening(ClientSession session, ScreeningInfo* info);
static void make_reservation(ClientSession session, ReservationInfo* info);
static void cancel_reservation(ClientSession session, ReservationInfo* info);

static void send_seating_info(ClientSession session, char* screening_id);
static void get_screenings(ClientSession session, char* movieId);
static void get_movies(ClientSession session);
static void get_reservations(ClientSession session, char * client, int cancelled);

static ListADT get_ticket_list(char* client, int cancelled);
static ListADT get_screening_list(char* movie);
static ListADT get_movie_list();


int new_thread(ClientSession client);


typedef struct ti{
    ClientSession session;
    int number;
} ThreadInfo;


DbSession database;

int main(int argc, char*argv[]){
    if (argc != 2){
        printf("Usage: server server-port-number");
        exit(1);
    }

    set_log();

    ServerInstance server = server_init(atoi(argv[1]));
   
    database = db_init();

    if(database == NULL){
        srv_log("[ERROR] Couldn't open database");
        exit(2);
    }

    synchro_init();

	while(1){
		ClientSession cli = wait_client(server);

        if(cli != NULL){
          srv_log("[CLIENT] New connection");
		  new_thread(cli);
       
        } else {
            srv_log("[WARNING] Unsucessful connection attempt");
        }
	}

    srv_log("Exiting...");
    return 1;
}


int num = 0;
int new_thread(ClientSession client){
    pthread_t thread;

    ThreadInfo* info = failfast_malloc(sizeof(ThreadInfo));
    info->session = client;
    info->number = num++;
    
    int creation = pthread_create(&thread, NULL, thread_work, info);
  
    if(creation == 0){
        pthread_detach(thread);
    }
    
    return creation;    
}


static void * thread_work(void* data){

    ThreadInfo* info = (ThreadInfo*)data;
    ClientSession session = info->session;
    free(info);
    
    while(1){

        ClientRequest * req = wait_request(session);
        if(req == NULL){
            //Hubo error en la recepcion, ignoro
            continue;
        }
        process_request(session,req);
        free(req);
     }

     
     return 0; 

}

static char* req_log_msg(char type){
    switch(type){
        case REQ_MOVIE_ADD:         return "Add movie";
        case REQ_MOVIE_DELETE:      return "Delete movie";
        case REQ_SCREENING_ADD:     return "Add screening";
        case REQ_SCREENING_DELETE:  return "Delete screening";
        case REQ_MAKE_RESERVATION:  return "Make reservation";
        case REQ_MOVIE_SCREENINGS:  return "Get screening list";
        case REQ_SEATING_INFO:      return "Get seating info";
        case REQ_MOVIE_LIST:        return "Get movie list";
        case REQ_CANCEL_RESERVATION:return "Cancel reservation";
        case REQ_RESERVATION_LIST:  return "Get ticket list";
        case REQ_CANCELLED_LIST:    return "Get cancelled tickets";

        default:                    return "Unknown";  
        }

}



static synchro_type get_synchro_type(char req_type){
    switch(req_type){
        case REQ_MOVIE_ADD:         return WRITER;
        case REQ_MOVIE_DELETE:      return WRITER;
        case REQ_SCREENING_ADD:     return WRITER;
        case REQ_SCREENING_DELETE:  return WRITER;
        case REQ_MAKE_RESERVATION:  return WRITER;
        case REQ_CANCEL_RESERVATION: return WRITER;
        
        case REQ_MOVIE_SCREENINGS:  return READER;
        case REQ_SEATING_INFO:      return READER;
        case REQ_MOVIE_LIST:        return READER;
        case REQ_RESERVATION_LIST:  return READER;

        default:                    return WRITER;  // No deberia entrar nunca, pero WRITER es mas defensivo 
        }

}

static void process_request(ClientSession session, ClientRequest* req){

    if(req == NULL){
      srv_log("[WARNING] Invalid client request. Ignoring...");
      client_send_error(session, INVALID_REQUEST);
    }

    
    srv_log("[CLIENT REQUEST] %s", req_log_msg(req->type));

    synchro_type r_w = get_synchro_type(req->type);
    enter_critical(r_w);

    switch(req->type){
        case REQ_MOVIE_ADD:         add_movie(session, (MovieInfo*)req->data);
                                    break;

        case REQ_MOVIE_DELETE:      delete_movie(session, (char*)req->data);
                                    break;

        case REQ_SCREENING_ADD:     add_screening(session, (ScreeningInfo*)req->data);
                                    break;

        case REQ_SCREENING_DELETE:  delete_screening(session, (ScreeningInfo*)req->data);
                                    break;

        case REQ_MAKE_RESERVATION:  make_reservation(session, (ReservationInfo*)req->data);
                                    break;

        case REQ_CANCEL_RESERVATION: cancel_reservation(session, (ReservationInfo*)req->data);
                                    break;

        case REQ_MOVIE_SCREENINGS:  get_screenings(session, (char*)req->data);
                                    break;

        case REQ_SEATING_INFO:      send_seating_info(session, (char*)req->data);
                                    break;
        
        case REQ_MOVIE_LIST:        get_movies(session);
                                    break;

        
        case REQ_RESERVATION_LIST:  get_reservations(session, (char*)req->data, 0);
                                    break;
                                    
        case REQ_CANCELLED_LIST:    get_reservations(session, (char*)req->data, 1);
                                    break;    
    }

    leave_critical(r_w);

    free(req->data);
}


static void do_stmnt_and_ack(ClientSession session, char* stmnt){
    int r = execute_statements(database, stmnt);
    if(r < 0){ // Error desde la base de datos
        error_t err;
        switch(r){
            case DB_ERR_CONSTRAINT: err = CONSTRAINT_VIOLATION; break;
            case DB_ERR_OTHER:      err = UNKNOWN_DB_ERROR;     break;
            default:                err = OTHER;                break;
        } 
        client_send_error(session, err);
    } else{
        client_send_ok(session);
    }

}


static int assert_existence(char* stmnt){
    QueryData q = new_query(stmnt, 1);
    do_query(database,q);
    char ** row = next_row(q);

    int count = atoi(row[0]);
    free(row);
    destroy_query_data(q);

    return (count != 0);
}


static void add_movie(ClientSession session, MovieInfo* info){
    #ifdef DEBUG
    sleep(10);
    #endif

    char * stmnt = failfast_malloc(MAX_QUERY_LEN + strlen(info->name) + strlen(info->description));
    sprintf(stmnt, STMNT_ADD_MOVIE, info->name, info->description);
    
    do_stmnt_and_ack(session, stmnt);
    
    free(stmnt);
}


static void delete_movie(ClientSession session, char* name){
    char * stmnt = failfast_malloc(MAX_QUERY_LEN + strlen(name));
    sprintf(stmnt, STMNT_DELETE_MOVIE, name);


    do_stmnt_and_ack(session, stmnt);
    free(stmnt);
}

static void add_screening(ClientSession session, ScreeningInfo* info){
    char * stmnt = failfast_malloc(MAX_QUERY_LEN + strlen(info->movie));

    sprintf(stmnt, QUERY_MOVIE_EXISTS, info->movie);
    if(!assert_existence(stmnt)){
        client_send_error(session, NO_SUCH_ELEMENT);
        free(stmnt);
        return;
    }


    sprintf(stmnt, STMNT_ADD_SCREENING, info->movie, info->day, info->month, info->slot, info->sala);


    do_stmnt_and_ack(session, stmnt);
    free(stmnt);
}


static void delete_screening(ClientSession session, ScreeningInfo* info){
    char * stmnt = failfast_malloc(MAX_QUERY_LEN);
    sprintf(stmnt, QUERY_SCREENING_EXISTS, atoi(info->id));

    if(!assert_existence(stmnt)){
        client_send_error(session, NO_SUCH_ELEMENT);
        free(stmnt);
        return;
    }

    sprintf(stmnt, STMNT_DELETE_SCREENING, atoi(info->id));
    
    do_stmnt_and_ack(session, stmnt);
    free(stmnt);
}


static void make_reservation(ClientSession session, ReservationInfo* info){
    char * stmnt = failfast_malloc(MAX_QUERY_LEN);

    sprintf(stmnt, QUERY_SCREENING_EXISTS, info->screening_id);

    if(!assert_existence(stmnt)){
        client_send_error(session, NO_SUCH_ELEMENT);
        free(stmnt);
        return;
    }


    sprintf(stmnt, STMNT_MAKE_RESERVATION, info->client, info->screening_id, info->seat);

    do_stmnt_and_ack(session, stmnt);
    free(stmnt);
}


static void cancel_reservation(ClientSession session, ReservationInfo* info){
    char * stmnt = failfast_malloc(MAX_QUERY_LEN);
    

    char * client = info->client;
    int seat = info->seat;
    int screening_id = info->screening_id;


    sprintf(stmnt, QUERY_RESERVATION_EXISTS, client, screening_id, seat);
    
    if(!assert_existence(stmnt)){
        client_send_error(session, NO_SUCH_ELEMENT);
        free(stmnt);
        return;
    }

    sprintf(stmnt, STMNT_CANCEL_AND_DELETE, client, screening_id, seat, 
                                            client, screening_id, seat);
    
    do_stmnt_and_ack(session, stmnt);
    free(stmnt);

}



static void get_screenings(ClientSession session, char* movieId){    
    ListADT s = get_screening_list(movieId);
    send_screenings(session, s);
    destroy_list(s);
}


static ListADT get_screening_list(char* movie){
    char * query = failfast_malloc(MAX_QUERY_LEN);

    sprintf(query, QUERY_GET_SCREENINGS, movie);

    QueryData q = new_query(query, 5);
    do_query(database, q);

    ListADT list = new_list(sizeof(ScreeningInfo));
    
    char ** row;

    while((row = next_row(q)) != NULL){
        ScreeningInfo s;

        strcpy(s.id, row[0]);
        s.day = atoi(row[1]); 
        s.month = atoi(row[2]); 
        s.slot = atoi(row[3]); 
        s.sala = atoi(row[4]); 
        
        add_to_list(list, &s);
        free(row);
    }

    free(query);
    destroy_query_data(q);
    return list;
}



static void get_movies(ClientSession session){
    ListADT m = get_movie_list();           
    send_movies(session, m);
    destroy_list(m);
}



static ListADT get_movie_list(){
    #ifdef DEBUG
    sleep(10);
    #endif
    char * query = failfast_malloc(MAX_QUERY_LEN);

    sprintf(query, QUERY_GET_MOVIE_LIST);

    QueryData q = new_query(query, 2);
    do_query(database, q);


    ListADT list = new_list(sizeof(MovieInfo));
    char ** row;

    while((row = next_row(q)) != NULL){
        MovieInfo info;

        strcpy(info.name, row[0]);
        strcpy(info.description, row[1]); 
        
        add_to_list(list, &info);

        free(row);
    }
    
    free(query);
    destroy_query_data(q);
    return list;
}



static void get_reservations(ClientSession session, char * client, int cancelled){
    ListADT t = get_ticket_list(client, cancelled);
    send_tickets(session, t);
    destroy_list(t);
}


static ListADT get_ticket_list(char* client, int cancelled){
    char * query = failfast_malloc(MAX_QUERY_LEN);

    if(cancelled){
        sprintf(query, QUERY_GET_CANCELLED, client);
    } else {
        sprintf(query, QUERY_GET_RESERVED, client);
    }

    QueryData q = new_query(query, 7);
    do_query(database, q);

    ListADT list = new_list(sizeof(Ticket));
   
    char** row;
    while((row = next_row(q)) != NULL){
        Ticket ticket;
        ticket.seat = atoi(row[0]);
        strcpy(ticket.screening.id, row[1]);
        strcpy(ticket.screening.movie, row[2]);
        ticket.screening.day = atoi(row[3]);
        ticket.screening.month = atoi(row[4]);
        ticket.screening.slot = atoi(row[5]);
        ticket.screening.sala = atoi(row[6]); 
    
        add_to_list(list, &ticket);

        free(row);
    }

    free(query);
    destroy_query_data(q);
    return list;
}

#define MAX_SEATS 300

static void send_seating_info(ClientSession session, char* screening_id){
    char * query = failfast_malloc(MAX_QUERY_LEN);

    sprintf(query, QUERY_OCCUPIED_SEATS, screening_id);
    QueryData q = new_query(query, 1);
    do_query(database, q);

    char* seats = failfast_calloc(MAX_SEATS,1);
    char** row;

    while((row = next_row(q)) != NULL){
        seats[atoi(row[0])] = 1;
        free(row);
    }

    send_seats(session, seats, MAX_SEATS);
    free(query);
    free(seats);
    destroy_query_data(q);
}



