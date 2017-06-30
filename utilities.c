#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char * safe_strncpy(char * destination, char * source, size_t n){
	strncpy(destination, source, n);
	destination[n] = '\0';
	return destination;
}


void * failfast_malloc(size_t size){
	void * ptr = malloc(size);
	if(ptr == NULL){
		fprintf(stderr, "[FATAL ERORR] No memory. Terminating.");
		exit(3);
	}

	return ptr;
}

void* failfast_realloc(void *ptr, size_t size){
	void * rptr = realloc(ptr, size);
	if(rptr == NULL){
		fprintf(stderr, "[FATAL ERROR] No memory. Terminating.");
		exit(3);
	}

	return rptr;
}

void * failfast_calloc(int num, size_t size){
	void * ptr = calloc(num, size);
	if(ptr == NULL){
		fprintf(stderr, "[FATAL ERORR] No memory. Terminating.");
		exit(3);
	}

	return ptr;
}
