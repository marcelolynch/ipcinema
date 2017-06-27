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
static void process_request(ClientRequest* req);
static void add_movie(MovieInfo* info);

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
        } else {
          process_request(req);
          client_send_ok(session);  

        }
    }
     
     return 0; 

}



static void process_request(ClientRequest* req){
    switch(req->type){
    case REQ_MOVIE_ADD:
        {
            srv_log("Will add movie");
            add_movie((MovieInfo*)req->data);
        }
        case REQ_MOVIE_DELETE:
        {
        }

        case REQ_SCREENING_ADD:
        {
        }

        case REQ_SCREENING_DELETE:
        {
        }

        default:
            break;
    }
}


static void add_movie(MovieInfo* info){
    char * stmnt = malloc(QUERY_LEN_OVERHEAD + strlen(info->name) + strlen(info->description));
    sprintf(stmnt, ADD_MOVIE, info->name, info->description);
    printf("Query: %s\n", stmnt);
    if(execute_statements(database, stmnt) < 0){
        srv_log("Error");
    } else {
        srv_log("Success");
    }
}