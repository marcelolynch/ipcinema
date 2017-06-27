#ifndef __callback_h
#define __callback_h


typedef struct querydataCDT * QueryData;

char ** deconcat(QueryData q, int cols);
int callback(void *res, int nrCols, char **colElems, char **colsName);

QueryData new_query_data(int cols);
void destroy_query_data(QueryData q);

//Devuelve la cantidad de columnas 
int n_cols(QueryData q);

//Devuelve la cantidad de filas
int n_rows(QueryData q);

char ** next_row(QueryData q);

#endif