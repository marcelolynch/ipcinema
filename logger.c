#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/prctl.h>
#include <time.h>

#include "log.h"

/* Proceso logger. Se utilizan System V Message Queues */


#define MSG_ANY 0


typedef void (*sighandler_t)(int);

//Message queue
int msqid;
key_t key;

// Logfile
FILE* logfile;

//Flags
int q_set; // Indica si la MQ está activa

void signal_handler(int signum){
  if(signum == SIGHUP){
    fprintf(stderr, "[LOG] Server process was killed. Logging process will close message queue and exit\n");
  } 
  if(q_set){
    //Cierro la message queue
    msgctl(msqid, IPC_RMID, NULL);
    q_set = 0;
    exit(3);
  }
  if(logfile != NULL){
    fclose(logfile);
    logfile = NULL;
  }
}


int main(int argc, char* argv[])
{
    prctl(PR_SET_PDEATHSIG, SIGHUP); // Control de orfandad: me suscribo a la muerte
                                     // de mi padre: me mandan un SIGHUP en ese caso
    
    // Alguien tiene que cerrar la message queue si las cosas se ponen
    // feas. Que sea este proceso y no el servidor: manejo estas señales.
    // No manejo SIGTERM: si me mata el servidor debería cerrar él la MQ.
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);


    struct log_msg buf;

    q_set = 0;

    if ((key = ftok(FTOK_PATH, FTOK_CHAR)) == -1
        || (msqid = msgget(key, 0644)) == -1) {
        fprintf(stderr, "[LOG] Logging process couldn't open the message queue and will exit");
        exit(1);
    }
    

    logfile = fopen("server.log", "a");
    
    if(logfile == NULL){
        fprintf(stderr, "[LOG] Logging process couldn't open logfile\n");
    } else{
        time_t t = time(0);
        char* timestamp = asctime(localtime(&t));
        fprintf(logfile,"\n\n ============================================================ \n NEW LOG INITIALIZATION: TIMESTAMP: %s ============================================================ \n\n", timestamp);
        fflush(logfile);
    }

    q_set = 1;

    printf("Successful log setup\n");

    while(1) { 
        if (msgrcv(msqid, &buf, MAX_MSG_LEN, MSG_ANY, 0) == -1) {
            fprintf(stderr, "[LOG] Error receiving from message queue (probably closed). Logging process will exit");
            exit(2);
        }

        printf("%s\n", buf.msg);
        fflush(stdout);

        if(logfile != NULL){
            fprintf(logfile, "%s\n", buf.msg);
        }
        
    }


  	
	return 0;
}