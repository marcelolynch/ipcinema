#ifndef __cineutil_h
#define __cineutil_h
#include <stdlib.h>

char * safe_strncpy(char * destination, char * source, size_t n);

void * failfast_malloc(size_t size);
void * failfast_realloc(void *ptr, size_t size);
void * failfast_calloc(int num, size_t size);



typedef struct listCDT * ListADT;
typedef struct iteratorCDT * ListIteratorADT;



ListADT new_list(size_t data_size);
void add_to_list(ListADT l, void* data);
void get_from_list(ListADT l, int index, void * data_buf);

void destroy_list(ListADT l);
size_t list_length(ListADT l);


ListIteratorADT get_iterator(ListADT l);
int iter_has_next(ListIteratorADT iter);
void iter_get_next(ListIteratorADT iter, void * data_buf);
void destroy_iterator(ListIteratorADT iter);



#endif