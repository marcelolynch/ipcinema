#ifndef __log_h
#define __log_h

#define MAX_MSG_LEN 512
#define FTOK_PATH "/tmp"
#define FTOK_CHAR 'X'

struct log_msg {
    long mtype;
    char msg[MAX_MSG_LEN];
};


#endif