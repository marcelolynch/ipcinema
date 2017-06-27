#ifndef __callback_h
#define __callback_h
/* res es un puntero a una instancia de la estructura Datos.*/
struct Datos {
              int rows;
              int cursor;
              char * data;
            };

typedef struct Datos *t_QueryData;

char ** deconcat(t_QueryData q, int cols);
int callback(void *res, int nrCols, char **colElems, char **colsName);

#endif