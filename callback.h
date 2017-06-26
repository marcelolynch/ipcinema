char *deconcat(char *info);
int callback(void *res, int nrCols, char **colElems, char **colsName);

/* res es un puntero a una instancia de la estructura Datos.*/
struct Datos {
              int rows;
              char * data;
            };
