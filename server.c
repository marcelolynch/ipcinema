#include "errors.h"
#include "db.h"
#include "queries.h"
#include "server.h"
#include "server_logging.h"
#include "server_marshalling.h"
#include "utilities.h"

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
#include <semaphore.h>


#define SERVER_PORT 12345 

static void * thread_work(void* data);
static int process_request(ClientSession session, ClientRequest* req);
static int add_movie(MovieInfo* info);
static int delete_movie(char* name);
static int add_screening(ScreeningInfo* info);
static int delete_screening(ScreeningInfo* info);
static int make_reservation(ReservationInfo* info);
static int send_seating_info(ClientSession session, char* screening_id);

static ScreeningDataList* get_screenings(char* movie);
static MovieInfoList* get_movies();

int new_thread(ClientSession client);


typedef struct ti{
    ClientSession session;
    int number;
} ThreadInfo;


pthread_mutex_t mtx;

DbSession database;

int main(int argc, char*argv[]){
    srand(time(0));
    if (argc != 2) fatal("Usage: server server-port-number");

    set_log();

    ServerInstance server = server_init(atoi(argv[1]));
   
    database = db_init();

    if(database == NULL){
        srv_log("[ERROR] Couldn't open database");
        exit(1);
    }

    pthread_mutex_init(&mtx, NULL);

	while(1){
		ClientSession cli = wait_client(server);

        if(cli != NULL){

          srv_log("New connection");
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

    while(1){
        
        ClientRequest * req = wait_request(info->session);
        if(req == NULL){
            client_send_error(session, INVALID_REQUEST);
            continue;
        } 
        else {

            pthread_mutex_lock(&mtx);
            int r = process_request(session,req); //Zona crítica
            pthread_mutex_unlock(&mtx);

            if(r > 0){
                client_send_ok(session); 
            }
            else{
                client_send_error(session, r);
            }
        }
     }
     return 0; 

}

static char* req_log_msg(char type){
    switch(type){
        case REQ_MOVIE_ADD:         return "[CLIENT REQUEST] Add movie";
        case REQ_MOVIE_DELETE:      return "[CLIENT REQUEST] Delete movie";
        case REQ_SCREENING_ADD:     return "[CLIENT REQUEST] Add screening";
        case REQ_SCREENING_DELETE:  return "[CLIENT REQUEST] Delete screening";
        case REQ_MAKE_RESERVATION:  return "[CLIENT REQUEST] Make reservation";
        case REQ_MOVIE_SCREENINGS:  return "[CLIENT REQUEST] Get screening list";
        case REQ_SEATING_INFO:      return "[CLIENT REQUEST] Get seating info";
        case REQ_MOVIE_LIST:        return "[CLIENT REQUEST] Get movie list";
   
        default:                    return "[CLIENT REQUEST] Unknown";  
        }

}

static int process_request(ClientSession session, ClientRequest* req){
    
    srv_log(req_log_msg(req->type));

    switch(req->type){
        case REQ_MOVIE_ADD:         return add_movie((MovieInfo*)req->data);
        case REQ_MOVIE_DELETE:      return delete_movie((char*)req->data);
        case REQ_SCREENING_ADD:     return add_screening((ScreeningInfo*)req->data);
        case REQ_SCREENING_DELETE:  return delete_screening((ScreeningInfo*)req->data);
        case REQ_MAKE_RESERVATION:  return make_reservation((ReservationInfo*)req->data);
        case REQ_MOVIE_SCREENINGS:  return send_screenings(session, get_screenings((char*)req->data));
        case REQ_SEATING_INFO:      return send_seating_info(session, (char*)req->data);
        case REQ_MOVIE_LIST:        return send_movies(session, get_movies());
        
        default:                    return -1;
    }
}


static int add_movie(MovieInfo* info){
    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD + strlen(info->name) + strlen(info->description));
    sprintf(stmnt, STMNT_ADD_MOVIE, info->name, info->description);
    printf("Query: %s\n", stmnt);
    
    return execute_statements(database, stmnt);
}


static int delete_movie(char* name){
    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD + strlen(name));
    sprintf(stmnt, STMNT_DELETE_MOVIE, name);

    printf("Query: %s\n", stmnt);

    return execute_statements(database, stmnt);

}

static int add_screening(ScreeningInfo* info){
    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD + strlen(info->movie));
    sprintf(stmnt, STMNT_ADD_SCREENING, info->movie, info->day, info->slot, 1);

    printf("Query: %s\n", stmnt);

    return execute_statements(database, stmnt);
}


static int delete_screening(ScreeningInfo* info){
    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD);
    sprintf(stmnt, STMNT_DELETE_SCREENING, info->day, info->slot, 1);
    
    printf("Query: %s\n", stmnt);

    return execute_statements(database, stmnt);
}


static int make_reservation(ReservationInfo* info){
    char * query = failfast_malloc(QUERY_LEN_OVERHEAD);

    sprintf(query, STMNT_MAKE_RESERVATION, info->client, info->screening_id, info->seat);

    int r = execute_statements(database, query);

    free(query);

    return r;
}


static ScreeningDataList* get_screenings(char* movie){
    char * query = failfast_malloc(QUERY_LEN_OVERHEAD);

    sprintf(query, QUERY_GET_SCREENINGS, movie);

    QueryData q = new_query(query, 4);
    do_query(database, q);

    char ** row;
    row = next_row(q);
    if(row == NULL){
        destroy_query_data(q);
        return NULL;
    }

    ScreeningDataList* list = failfast_malloc(sizeof(*list));
    
    strcpy(list->data.id, row[0]);
    list->data.day = atoi(row[1]); 
    list->data.slot = atoi(row[2]); 
    list->data.sala = atoi(row[3]); 
 
    list->next = NULL;

    ScreeningDataList* p = list;

    while((row = next_row(q)) != NULL){
        p->next = failfast_malloc(sizeof(*p));
        p = p->next;
        strcpy(p->data.id, row[0]);
        p->data.day = atoi(row[1]); 
        p->data.slot = atoi(row[2]); 
        p->data.sala = atoi(row[3]); 
 
        
        p->next = NULL;
    }

    destroy_query_data(q);
    return list;
}


static MovieInfoList* get_movies(){
    char * query = failfast_malloc(QUERY_LEN_OVERHEAD);

    sprintf(query, QUERY_GET_MOVIE_LIST);

    QueryData q = new_query(query, 2);
    do_query(database, q);

    char ** row;
    row = next_row(q);
    if(row == NULL){
        destroy_query_data(q);
        return NULL;
    }

    MovieInfoList* list = failfast_malloc(sizeof(*list));
    
    strcpy(list->info.name, row[0]);
    strcpy(list->info.description, row[1]); 
    list->next = NULL;

    MovieInfoList* p = list;

    while((row = next_row(q)) != NULL){
        p->next = failfast_malloc(sizeof(*p));
        p = p->next;
      
        strcpy(p->info.name, row[0]);
        strcpy(p->info.description, row[1]); 
        
        p->next = NULL;
    }
    
    destroy_query_data(q);
    return list;
}



static int send_seating_info(ClientSession session, char* screening_id){
    char * query = failfast_malloc(QUERY_LEN_OVERHEAD);

    sprintf(query, QUERY_OCCUPIED_SEATS, screening_id);
    QueryData q = new_query(query, 1);
    do_query(database, q);

    char* seats = calloc(100,1);
    char** row;
    while((row = next_row(q)) != NULL){
        seats[atoi(row[0])] = 1;
    }

    send_seats(session, seats);
    free(query);
    free(seats);
    return 1;
}



