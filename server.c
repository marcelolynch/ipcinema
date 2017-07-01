#include "errors.h"
#include "db.h"
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


#define SERVER_PORT 12345 

static void * thread_work(void* data);
static int process_request(ClientSession session, ClientRequest* req);
static int add_movie(MovieInfo* info);
static int delete_movie(char* name);
static int add_screening(ScreeningInfo* info);
static int delete_screening(ScreeningInfo* info);
static int make_reservation(ReservationInfo* info);
static int send_seating_info(ClientSession session, char* screening_id);
static int get_screenings(ClientSession session, char* movieId);
static int get_movies(ClientSession session);

static ScreeningDataList* get_screening_list(char* movie);
void destroy_screening_list(ScreeningDataList* list);

static MovieInfoList* get_movie_list();
void destroy_movie_list(MovieInfoList* list);


int new_thread(ClientSession client);


typedef struct ti{
    ClientSession session;
    int number;
} ThreadInfo;


DbSession database;

int main(int argc, char*argv[]){
    if (argc != 2) fatal("Usage: server server-port-number");

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
          srv_log("New connection");
          destroy_log();
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
            client_send_error(session, INVALID_REQUEST);
            continue;
        } 
        else {

           // pthread_mutex_lock(&mtx);
            int r = process_request(session,req); //Zona crítica
           // pthread_mutex_unlock(&mtx);

            if(r > 0){
                client_send_ok(session); 
            }
            else{
                client_send_error(session, r);
            }
        }
        free(req->data);
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
        
        case REQ_MOVIE_SCREENINGS:  return READER;
        case REQ_SEATING_INFO:      return READER;
        case REQ_MOVIE_LIST:        return READER;

        default:                    return WRITER;  // No deberia entrar nunca, pero WRITER es mas defensivo 
        }

}

static int process_request(ClientSession session, ClientRequest* req){
    
    srv_log("[CLIENT REQUEST] %s", req_log_msg(req->type));

    synchro_type r_w = get_synchro_type(req->type);
    enter_critical(r_w);

    int ret_val;
    switch(req->type){
        case REQ_MOVIE_ADD:         ret_val = add_movie((MovieInfo*)req->data);
            break;

        case REQ_MOVIE_DELETE:      ret_val = delete_movie((char*)req->data);
            break;

        case REQ_SCREENING_ADD:     ret_val = add_screening((ScreeningInfo*)req->data);
            break;

        case REQ_SCREENING_DELETE:  ret_val = delete_screening((ScreeningInfo*)req->data);
            break;

        case REQ_MAKE_RESERVATION:  ret_val = make_reservation((ReservationInfo*)req->data);
            break;

        case REQ_MOVIE_SCREENINGS:  ret_val = get_screenings(session, (char*)req->data);
            break;

        case REQ_SEATING_INFO:      ret_val = send_seating_info(session, (char*)req->data);
            break;
        
        case REQ_MOVIE_LIST:       ret_val = get_movies(session);
                break;
        
        default:                    return -1;
    }

    leave_critical(r_w);
    return ret_val;
}


static int add_movie(MovieInfo* info){
    #ifdef DEBUG
    sleep(10);
    #endif

    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD + strlen(info->name) + strlen(info->description));
    sprintf(stmnt, STMNT_ADD_MOVIE, info->name, info->description);
    
    int r = execute_statements(database, stmnt);
    free(stmnt);
    return r;
}


static int delete_movie(char* name){
    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD + strlen(name));
    sprintf(stmnt, STMNT_DELETE_MOVIE, name);


    int r = execute_statements(database, stmnt);
    free(stmnt);
    return r;

}

static int add_screening(ScreeningInfo* info){
    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD + strlen(info->movie));
    sprintf(stmnt, STMNT_ADD_SCREENING, info->movie, info->day, info->month, info->slot, info->sala);


    int r = execute_statements(database, stmnt);
    free(stmnt);
    return r;
}


static int delete_screening(ScreeningInfo* info){
    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD);
    sprintf(stmnt, STMNT_DELETE_SCREENING, info->day, info->month, info->slot, info->sala);
    
    int r = execute_statements(database, stmnt);
    free(stmnt);
    return r;
}


static int make_reservation(ReservationInfo* info){
    char * stmnt = failfast_malloc(QUERY_LEN_OVERHEAD);

    sprintf(stmnt, STMNT_MAKE_RESERVATION, info->client, info->screening_id, info->seat);

    int r = execute_statements(database, stmnt);
    free(stmnt);
    return r;
}


static int get_screenings(ClientSession session, char* movieId){
    ScreeningDataList* s = get_screening_list(movieId);
    
    int ret_val = send_screenings(session, s);
    
    destroy_screening_list(s);
    return ret_val;
}


static ScreeningDataList* get_screening_list(char* movie){
    char * query = failfast_malloc(QUERY_LEN_OVERHEAD);

    sprintf(query, QUERY_GET_SCREENINGS, movie);

    QueryData q = new_query(query, 5);
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
    list->data.month = atoi(row[2]); 
    list->data.slot = atoi(row[3]); 
    list->data.sala = atoi(row[4]); 
    free(row);

    list->next = NULL;

    ScreeningDataList* p = list;

    while((row = next_row(q)) != NULL){
        p->next = failfast_malloc(sizeof(*p));
        p = p->next;
        strcpy(p->data.id, row[0]);
        p->data.day = atoi(row[1]); 
        p->data.month = atoi(row[2]); 
        p->data.slot = atoi(row[3]); 
        p->data.sala = atoi(row[4]); 
 
        
        p->next = NULL;

        free(row);
    }

    free(query);
    destroy_query_data(q);
    return list;
}


void destroy_screening_list(ScreeningDataList* list){
    ScreeningDataList* l;
    while(list != NULL){
        l = list;
        list = list->next;
        free(l);
    }
}

 
 static int get_movies(ClientSession session){
    MovieInfoList* m = get_movie_list();           
    int ret_val = send_movies(session, m);
    destroy_movie_list(m);

    return ret_val;
}
static MovieInfoList* get_movie_list(){
    #ifdef DEBUG
    sleep(10);
    #endif
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
    free(row);
    
    list->next = NULL;

    MovieInfoList* p = list;

    while((row = next_row(q)) != NULL){
        p->next = failfast_malloc(sizeof(*p));
        p = p->next;
      
        strcpy(p->info.name, row[0]);
        strcpy(p->info.description, row[1]); 
        
        p->next = NULL;
        free(row);
    }
    
    free(query);
    destroy_query_data(q);
    return list;
}


void destroy_movie_list(MovieInfoList* list){
    MovieInfoList* l;
    while(list != NULL){
        l = list;
        list = list->next;
        free(l);
    }
}



#define MAX_SEATS 300

static int send_seating_info(ClientSession session, char* screening_id){
    char * query = failfast_malloc(QUERY_LEN_OVERHEAD);

    sprintf(query, QUERY_OCCUPIED_SEATS, screening_id);
    QueryData q = new_query(query, 1);
    do_query(database, q);

    char* seats = failfast_calloc(MAX_SEATS,1);
    char** row;
    while((row = next_row(q)) != NULL){
        seats[atoi(row[0])] = 1;
        free(row);
    }

    send_seats(session, seats);
    free(query);
    free(seats);
    destroy_query_data(q);
    return 1;
}



