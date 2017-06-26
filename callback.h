/* res es un puntero a una instancia de la estructura Datos.
struct Datos {
              int filas;
              char * data;
             }
struct Datos res;
*/

int callback(void *res, int nroFilas, char **colElems, char **colsName){

  res->rows = nroFilas;

  if (argc > 0 ){
    strcopy(res->data,colElems[0]);
  for (int i = 0; i < nroFilas ; i++)
    strcat(res->data,"|");
    strcat(res->data,colElems[i]);
  }
  return 0;
}
