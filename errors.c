#include <stdio.h>
#include <stdlib.h>
#include "server_logging.h"

void fatal(char* msg){
	printf("%s",msg);
    exit(1);
}
