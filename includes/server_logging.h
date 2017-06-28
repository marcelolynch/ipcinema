#ifndef __srv_log
#define __srv_log


void set_log();
void srv_log(char * msg);
void destroy_log();

void log_socket(int code, int sending);


#endif