#include "errors.h"
#include "db.h"
#include "queries.h"
#include "server.h"
#include "server_logging.h"
#include "server_marshalling.h"

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
static ScreeningDataList* get_screenings(char* movie);

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

    ThreadInfo* info = malloc(sizeof(ThreadInfo));
    info->session = client;
    info->number = num++;
    
    return pthread_create(&thread, NULL, thread_work, info);

}


static void * thread_work(void* data){

    ThreadInfo* info = (ThreadInfo*)data;
    ClientSession session = info->session;

    while(1){
        
        ClientRequest * req = wait_request(info->session);
        if(req == NULL){
            client_send_error(session);
            continue;
        } 
        else {
            if(process_request(session, req) > 0){
                client_send_ok(session);  
            }
            else{
                client_send_error(session);
            }

        }
    }
     
     return 0; 

}

static int process_request(ClientSession session, ClientRequest* req){
    switch(req->type){
    case REQ_MOVIE_ADD:
        {
            srv_log("[CLIENT REQUEST] Add movie");
            return add_movie((MovieInfo*)req->data);
            break;
        }
        case REQ_MOVIE_DELETE:
        {
            srv_log("[CLIENT REQUEST] Delete movie");
            return delete_movie((char*)req->data);
            break;
        }

        case REQ_SCREENING_ADD:
        {
            srv_log("[CLIENT REQUEST] Screening add");
            return add_screening((ScreeningInfo*)req->data);
            break;
        }

        case REQ_SCREENING_DELETE:
        {
            srv_log("[CLIENT REQUEST] Screening delete");
            return delete_screening((ScreeningInfo*)req->data);
            break;
        }
        case REQ_MAKE_RESERVATION:
        {
            srv_log("[CLIENT REQUEST] Make reservation");
            return make_reservation((ReservationInfo*)req->data);
            break;

        }
        case REQ_MOVIE_SCREENINGS:
        {
            srv_log("[CLIENT REQUEST] Movie screenings");
            return send_screenings(session, get_screenings((char*)req->data));
        }
        default:
            return -1;
            break;
    }
}


static int add_movie(MovieInfo* info){
    char * stmnt = malloc(QUERY_LEN_OVERHEAD + strlen(info->name) + strlen(info->description));
    sprintf(stmnt, ADD_MOVIE, info->name, info->description);
    printf("Query: %s\n", stmnt);
    
    return execute_statements(database, stmnt);
}


static int delete_movie(char* name){
    char * stmnt = malloc(QUERY_LEN_OVERHEAD + strlen(name));
    sprintf(stmnt, DELETE_MOVIE, name);

    printf("Query: %s\n", stmnt);

    return execute_statements(database, stmnt);

}

static int add_screening(ScreeningInfo* info){
    char * stmnt = malloc(QUERY_LEN_OVERHEAD + strlen(info->movie));
    sprintf(stmnt, ADD_SCREENING, info->movie, info->day, info->slot, 1);

    printf("Query: %s\n", stmnt);

    return execute_statements(database, stmnt);
}


static int delete_screening(ScreeningInfo* info){
    char * stmnt = malloc(QUERY_LEN_OVERHEAD);
    sprintf(stmnt, DELETE_SCREENING, info->day, info->slot, 1);
    
    printf("Query: %s\n", stmnt);

    return execute_statements(database, stmnt);
}


static int make_reservation(ReservationInfo* info){
    char * query = malloc(QUERY_LEN_OVERHEAD);

    sprintf(query, "INSERT INTO RESERVA(cliente, proyeccionId, asiento, estado) VALUES ('%s', %d, %d, 'Reservado')", info->client, info->screening_id, info->seat);

    int r = execute_statements(database, query);

    free(query);

    return r;
}


static ScreeningDataList* get_screenings(char* movie){
    char * query = malloc(QUERY_LEN_OVERHEAD);

    sprintf(query, "SELECT id,dia,slot,sala FROM Proyeccion WHERE nombrePelicula = '%s';", movie);

    QueryData q = new_query(query, 4);
    do_query(database, q);

    char ** row;
    row = next_row(q);
    if(row == NULL)
        return NULL;

    ScreeningDataList* list = malloc(sizeof(*list));
    
    strcpy(list->data.id, row[0]);
    list->data.day = atoi(row[1]); 
    list->data.slot = atoi(row[2]); 
    list->data.sala = atoi(row[3]); 
 

    list->next = NULL;

    ScreeningDataList* p = list;

    while((row = next_row(q)) != NULL){
        p->next = malloc(sizeof(*p));
        p = p->next;
        strcpy(p->data.id, row[0]);
        p->data.day = atoi(row[1]); 
        p->data.slot = atoi(row[2]); 
        p->data.sala = atoi(row[3]); 
 
        
        p->next = NULL;
    }

    return list;
}