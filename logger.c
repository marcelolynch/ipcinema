#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUF 1024

#define FIFO_PATH "/tmp/myfifo"

int main(int argc, char* argv[])
{
    int fd;
    char buf[MAX_BUF];

    mkfifo(FIFO_PATH, 0666);

    
    while(1){
	  fd = open(FIFO_PATH, O_RDWR);
  	  read(fd, buf, MAX_BUF);
   	  printf("[LOG]: %s\n", buf);
      close(fd);
    }
	
	return 0;
}
