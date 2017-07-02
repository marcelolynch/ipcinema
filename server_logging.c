#include "server_logging.h"
#include "protocol.h"
#include "utilities.h"
#include "log.h"

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

#define LOG_PROC "logger.bin"

void log_socket(int code, int sending);
char* opcode_to_str(int code);

int log_set;
int msqid;
key_t key;

int logger_pid;

void set_log(){

    if ((key = ftok(FTOK_PATH, FTOK_CHAR)) == -1
        || (msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {
        fprintf(stderr, "Couldn't set up log. Will continue without logging functions\n");
        log_set = 0;
        return;
    } 

    int pid;
    if((pid = fork()) == 0){
        execl(LOG_PROC, LOG_PROC, NULL);
    }

    logger_pid = pid;
    log_set = 1;
}

void srv_log(const char * fmt, ...){
    if(!log_set){
        return;
    }
    struct log_msg message;

    va_list args;
    va_start(args, fmt);
    vsprintf(message.msg, fmt, args);
    va_end(args);

    message.mtype = 1;
    msgsnd(msqid, &message, MAX_MSG_LEN, 0);

}


void destroy_log(){
    if(log_set){
        kill(logger_pid, SIGTERM); // Mato a mi hjo
        msgctl(msqid, IPC_RMID, NULL); // Cierro la MQ
        log_set = 0;
    }
}


void log_socket(int code, int sending){
    FILE* f = fopen("socketlog.txt", "a");
    char * msg = sending ? "Sending" : "Received";

    fprintf(f, "[NETWORK] %s %s\n", msg, opcode_to_str(code));
    fclose(f);
}


char* opcode_to_str(int code){
    switch(code){
        case OK: return "OK";
        case ERROR: return "ERROR";
        case TRANSACTION_BEGIN: return "TRANSACTION_BEGIN";
        case TRANSACTION_END:   return "TRANSACTION_END";
        case TRANSACTION_NEXT: return "TRANSACTION_NEXT";
        case TRANSACTION_ITEM: return "TRANSACTION_ITEM";

        case MOVIE_ADD: return "MOVIE_ADD";
        case MOVIE_DELETE: return "MOVIE_DELETE";
        case SCREENING_ADD: return "SCREENING_ADD";
        case SCREENING_DELETE: return "SCREENING_DELETE";
        case SCREENING_INFO: return "SCREENING_INFO";
        case MAKE_RESERVATION: return "MAKE_RESERVATION";
        case MOVIE_SCREENINGS: return "MOVIE_SCREENINGS";
        default: return "";
    }

    return "";
}