/* res es un puntero a una instancia de la estructura Datos.*/
struct Datos {
              int filas;
              char * data;
             }
struct Datos res;

/*deconcat te devuelve el primer elemento del string concatenado y lo elimina del string*/
string *deconcat(string *info){
    for (int i= 0 ; info[i] !='|' && info[i] != '\0' ; i++);
    strncpy(char* res, info, i);
    i++;
    for (int j = 0 ; info[j] != '\0' ; j++ )
    info[j]= info[j+i];
    info[j]='\0';
    return res;
}

int callback(void *res, int nrCols, char **colElems, char **colsName){

  res->rows = res->rows++;

  if (strlen(res->data)==0){
    strcopy(res->data,colElems[0]);
  for (int i = 0; i < nrCols ; i++)
    strcat(res->data,"|");
    strcat(res->data,colElems[i]);
  }
  return 0;
}
