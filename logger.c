#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/prctl.h>

#include "log.h"


#define MSG_ANY 0


typedef void (*sighandler_t)(int);

//Message queue
int msqid;
key_t key;

int q_set;

void parent_killed_handler(int signum){
  fprintf(stderr, "[LOG] Server process was killed. Logging process will exit\n");
  if(q_set){
    //Cierro la message queue
    msgctl(msqid, IPC_RMID, NULL);
    exit(3);
  }
}


int main(int argc, char* argv[])
{
    prctl(PR_SET_PDEATHSIG, SIGHUP); // Zombie control
    signal(SIGHUP, parent_killed_handler);

    struct log_msg buf;

    q_set = 0;

    if ((key = ftok(FTOK_PATH, FTOK_CHAR)) == -1
        || (msqid = msgget(key, 0644)) == -1) {
        fprintf(stderr, "[LOG] Logging process couldn't open the message queue and will exit");
        exit(1);
    }
    return 1;
    
    q_set = 1;

    printf("Successful log setup\n");

    while(1) { 
        if (msgrcv(msqid, &buf, MAX_MSG_LEN, MSG_ANY, 0) == -1) {
            fprintf(stderr, "[LOG] Error receiving from message queue (probably closed). Logging process will exit");
            exit(2);
        }

        printf("%s\n", buf.msg);
        fflush(stdout);
    }


  	
	return 0;
}
