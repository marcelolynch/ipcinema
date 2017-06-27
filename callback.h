#ifndef __callback_h
#define __callback_h

/* res es un puntero a una instancia de la estructura Datos.*/
struct Datos {
              int rows;
              int cols;
              int cursor;
              char* ** data;
              size_t size;
              size_t capacity;
            };

typedef struct Datos * QueryData;

char ** deconcat(QueryData q, int cols);
int callback(void *res, int nrCols, char **colElems, char **colsName);
QueryData new_query_data(int cols);
char ** next_row(QueryData q);

#endif