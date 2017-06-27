#include "errors.h"
#include "db.h"
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


    char* insertions = "INSERT INTO Pelicula VALUES ('Matrix', 'Matrix la pelicula' );"
                  "INSERT INTO Pelicula VALUES ('Harry Potter', 'La piedra filosofal' );"
                  "INSERT INTO Pelicula VALUES ('El senor de los Anillos', 'La primera' );"
                  "INSERT INTO Proyeccion(nombrePelicula, dia, slot, sala) VALUES ('Matrix', 2, 3, 4);"
                  "INSERT INTO Proyeccion(nombrePelicula, dia, slot, sala) VALUES ('Harry Potter', 2, 4, 4);";


    execute_statements(database, insertions);

    QueryData q = new_query("SELECT * FROM pelicula", 2);

    do_query(database, q);
    char** row;

    while((row = next_row(q)) != NULL){
        printf("\n");
        for (int j = 0 ; j < n_cols(q) ; j++){
            printf("%s  ", row[j] ? row[j] : "NULL");
        }

    }
    fflush(stdout);

    while(1){
        
        ClientRequest * req = wait_request(info->session);
        if(req == NULL){
            client_send(session, "Not valid dude\n");
            continue;
        } else {
          client_send(session, "OK\n");  
        }
    }
     
     return 0; 

}