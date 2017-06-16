#include "sockets.h"


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

	Address srv = open_socket("localhost", SERVER_PORT);
	socket_bind(srv);

	while(1){
		Connection con = accept_connection(srv);
		new_thread(con);
	}

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
        receive_message(info->connection,buffer);
        printf("Here is the message: %s\n",buffer);
        send_message(info->connection,"I got your message");
    }
     
     return 0; 

}
