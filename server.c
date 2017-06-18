#include "errors.h"
#include "fakedb.h"
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
static void reservation(ClientSession session, ReservationRequest request);

int new_thread(ClientSession client);


typedef struct ti{
    ClientSession session;
    int number;
} ThreadInfo;


pthread_mutex_t mtx;

int main(int argc, char*argv[]){
    srand(time(0));
    if (argc != 2) fatal("Usage: server server-port-number");

    ServerInstance server = server_init("localhost", atoi(argv[1]));
   
    pthread_mutex_init(&mtx, NULL);

	while(1){
		ClientSession cli = wait_client(server);
        printf("New connection\n");
		new_thread(cli);
	}

    printf("exiting");
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

    int n, waiting=1;
    ThreadInfo* info = (ThreadInfo*)data;
    ClientSession session = info->session;

    while(1){
        
        ClientRequest * req = wait_request(info->session);
        if(req == NULL){
            client_send(session, "Not valid dude\n");
            continue;
        } 

        switch(req->type){
            case RESERVATION:
            {
                ReservationRequest res = (ReservationRequest)req->data;
                reservation(session, res);
                break;
            }
            case SEAT_INFO:
                break;
        }




    }
     
     return 0; 

}


static void reservation(ClientSession session, ReservationRequest request){
        int status;
                
        sleep(3+rand()%10);  //Testing

        pthread_mutex_lock(&mtx); //Enter critical zone
        status = reserve_seat(request->auditorium, request->seat);
        pthread_mutex_unlock(&mtx); //Exit critical zone
        
        if(status < 0){
            client_send(session, "Failure: seat was reserved\n");
        } else{
            client_send(session,"Success!\n");
        }
                
}