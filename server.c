#include "sockets.h"
#include "errors.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

#define SERVER_PORT 12345 

static void * thread_work(void* data);
int new_thread(Connection con);

typedef struct ti{
    Connection connection;
} ThreadInfo;


int main(int argc, char*argv[]){
    if (argc != 2) fatal("Usage: server server-port-number");

	Address srv = open_socket("localhost", atoi(argv[1]));
	socket_bind(srv);

	while(1){
		Connection con = accept_connection(srv);
        printf("New connection\n");
		new_thread(con);
	}

    printf("exiting");
}


int new_thread(Connection con){
    pthread_t thread;

    ThreadInfo* info = malloc(sizeof(ThreadInfo));
    info->connection = con;
    
    return pthread_create(&thread, NULL, thread_work, info);

}


static void * thread_work(void* data){
    char buffer[1024];

    int n;
    ThreadInfo* info = (ThreadInfo*)data;
 
    while(1){
        bzero(buffer,1024);
        if(receive_message(info->connection,buffer)){
            printf("Here is the message: %s\n",buffer);
            send_message(info->connection,"I got your message");
        } else{
            printf("Broken pipe");
            break;
        }
    }
     
     return 0; 

}
