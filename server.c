#include "errors.h"
#include "fakedb.h"
#include "sockets.h"
#include "server.h"
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
int new_thread(Connection con);


typedef struct ti{
    Connection connection;
    int number;
} ThreadInfo;


sem_t mtx;

int main(int argc, char*argv[]){
    srand(time(0));
    if (argc != 2) fatal("Usage: server server-port-number");

	Address srv = open_socket("localhost", atoi(argv[1]));
	socket_bind(srv);

    sem_init(&mtx, 0, 1);

	while(1){
		Connection con = accept_connection(srv);
        printf("New connection\n");
		new_thread(con);
	}

    printf("exiting");
}

int num = 0;
int new_thread(Connection con){
    pthread_t thread;

    ThreadInfo* info = malloc(sizeof(ThreadInfo));
    info->connection = con;
    info->number = num++;
    
    return pthread_create(&thread, NULL, thread_work, info);

}


static void * thread_work(void* data){

    int n, waiting=1;
    ThreadInfo* info = (ThreadInfo*)data;
    
    while(1){
        
        ClientRequest * req = wait_request(info->connection);
        if(req == NULL){
            continue;
        } 

        switch(req->type){
            case RESERVATION:
            {
                ReservationRequest res = (ReservationRequest)req->data;
                int status;
                
                sleep(1+rand()%3);
                sem_wait(&mtx); //Enter critical zone
                status = reserve_seat(res->auditorium, res->seat);
                sem_post(&mtx); //Exit critical zone

                if(status < 0){
                    send_message(info->connection, "Failure: seat was reserved\n");
                } else{
                    send_message(info->connection,"Success!\n");
                }
                
                break;
            }
            case SEAT_INFO:
                break;
        }




    }
     
     return 0; 

}
