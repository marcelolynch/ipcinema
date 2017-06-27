#include "server_logging.h"

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define FIFO_PATH "/tmp/myfifo"
#define LOG_PROC "logger.bin"

void set_log()
{
    mkfifo(FIFO_PATH, 0666);
  
    if(fork() == 0){
        execl(LOG_PROC, LOG_PROC, NULL);
    }

}


void srv_log(char * msg){
//	printf("%s\n", msg);
   int fd = open(FIFO_PATH, O_WRONLY);
    write(fd, msg, strlen(msg)+1);
    close(fd);

}


void destroy_log(){
	unlink(FIFO_PATH);
}