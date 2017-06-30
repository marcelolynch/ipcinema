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
		fprintf(stderr, "No memory. Terminating.");
		exit(3);
	}

	return ptr;
}