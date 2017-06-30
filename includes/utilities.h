#ifndef __cineutil_h
#define __cineutil_h
#include <stdlib.h>

char * safe_strncpy(char * destination, char * source, size_t n);

void * failfast_malloc(size_t size);
void * failfast_realloc(void *ptr, size_t size);
void * failfast_calloc(int num, size_t size);

#endif