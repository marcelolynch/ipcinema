#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utilities.h"

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



// Linked list: solo se puede agregar, no necesito más
typedef struct node {
	void * data;
	struct node * next;
} Node;


struct listCDT{
	Node * first;
	Node * last;
	size_t length;
	size_t bytes;
};


struct iteratorCDT {
	Node * ptr;
	size_t bytes;
};

ListADT new_list(size_t data_size){
	ListADT l = failfast_malloc(sizeof *l);
	l->first = NULL;
	l->last = NULL;
	l->length = 0;
	l->bytes = data_size;

	return l;
}


void add_to_list(ListADT l, void* data){
	if(l->first == NULL){
		l->first = failfast_malloc(sizeof *l->first);
		l->last = l->first;
	} else {
		l->last->next = failfast_malloc(l->bytes);
		l->last = l->last->next;
	}
	
	l->last->data = failfast_malloc(l->bytes);
	memcpy(l->last->data, data, l->bytes);
	l->last->next = NULL;
	l->length++;
}


size_t list_length(ListADT l){
	return l->length;
}

void destroy_list(ListADT l){
	Node* n;
	while(l->first != NULL){
		n = l->first;
		l->first = l->first->next;
		free(n->data);
		free(n);
	}
	free(l);
}


ListIteratorADT get_iterator(ListADT l){
	ListIteratorADT iter = failfast_malloc(sizeof *iter);
	iter->ptr = l->first;
	iter->bytes = l->bytes;

	return iter;
}



int iter_has_next(ListIteratorADT iter){
	return iter->ptr != NULL;
}


void iter_get_next(ListIteratorADT iter, void * data_buf){
	if(!iter_has_next(iter)){
		return;
	}
	memcpy(data_buf, iter->ptr->data, iter->bytes);
	iter->ptr = iter->ptr->next;
}


void destroy_iterator(ListIteratorADT iter){
	free(iter);
}