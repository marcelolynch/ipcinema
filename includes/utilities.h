#ifndef __cineutil_h
#define __cineutil_h
#include <stdlib.h>

char * safe_strncpy(char * destination, char * source, size_t n);

void * failfast_malloc(size_t size);
void * failfast_realloc(void *ptr, size_t size);
void * failfast_calloc(int num, size_t size);


/*	ListADT implementa una lista simplemente encadenada
	con inserción al final O(1), consulta de i-ésimo elemento (O(N)),
	y su tamaño O(1) */
typedef struct listCDT * ListADT;


/* 
	Construye una nueva lista, vacía. data_size especifica el tamaño en bytes
	de los datos que se guardarán.
 */
ListADT new_list(size_t data_size);

/*Agrega el elemento al final de la lista*/
void add_to_list(ListADT l, void* data);

/* 
	Recupera el elemento index (desde el principio) de la lista 
	y lo copia a la zona de memoria apuntada por data_buf.
*/
void get_from_list(ListADT l, int index, void * data_buf);

/* Retorna el tamaño actual de la lista */
size_t list_length(ListADT l);

/* Libera recursos. La lista no debería utilizarse luego */
void destroy_list(ListADT l);



/*
	Iterador para ListADT.	
*/
typedef struct iteratorCDT * ListIteratorADT;


/* Construye un iterador para la lista l. Se itera desde 
el primer elemento hasta el último */
ListIteratorADT get_iterator(ListADT l);


/* Devuelve 1 si todavía quedan elementos por iterar, 0 si no */
int iter_has_next(ListIteratorADT iter);

/* Copia en la zona de memoria apuntada por data_buf el siguiente elemento
del iterador. Si no existe tal elemento se termina el programa (!) */
void iter_get_next(ListIteratorADT iter, void * data_buf);

/* Libera recursos */
void destroy_iterator(ListIteratorADT iter);



#endif