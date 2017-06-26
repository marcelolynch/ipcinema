#include <string.h>
#include <stdlib.h>
#include "callback.h"

/*deconcat te devuelve el primer elemento del string concatenado y lo elimina del string*/


char *deconcat(char *info){
    char *res = malloc (10000*sizeof(char*));	
    int i, j;
    for (i = 0 ; info[i] !='|' && info[i] != '\0' ; i++);
    strncpy(res, info, i);
    i++;
    for (j = 0 ; info[j] != '\0' ; j++ )
    info[j]= info[j+i];
    info[j]='\0';
    return res;
}

int callback(void *res, int nrCols, char **colElems, char **colsName){
  int i;
  struct Datos *result = malloc (10000*sizeof(char*));
  result= res;
  result->rows = result->rows++;

  if (strlen(result->data)==0){
    strcpy(result->data,colElems[0]);
  for (i = 0; i < nrCols ; i++)
    strcat(result->data,"|");
    strcat(result->data,colElems[i]);
  }
  return 0;
}
