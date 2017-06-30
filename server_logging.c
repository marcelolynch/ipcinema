#include "server_logging.h"
#include "protocol.h"

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

#define FIFO_PATH "/tmp/myfifo"
#define LOG_PROC "logger.bin"

void log_socket(int code, int sending);
char* opcode_to_str(int code);

void set_log()
{
    mkfifo(FIFO_PATH, 0666);
  
    if(fork() == 0){
        execl(LOG_PROC, LOG_PROC, NULL);
    }

}

void srv_log(const char * fmt, ...){

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");

    fflush(stdout);

   /*int fd = open(FIFO_PATH, O_WRONLY);
    write(fd, msg, strlen(msg)+1);
    close(fd);
*/
}


void destroy_log(){
	unlink(FIFO_PATH);
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
        case MOVIE_INFO: return "MOVIE_INFO";
        case MAKE_RESERVATION: return "MAKE_RESERVATION";
        case MOVIE_SCREENINGS: return "MOVIE_SCREENINGS";
        default: return "";
    }

    return "";
}