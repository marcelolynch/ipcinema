#ifndef __cineutil_h
#define __cineutil_h
#include <stdlib.h>

char * safe_strncpy(char * destination, char * source, size_t n);

void * failfast_malloc(size_t size);

#endif