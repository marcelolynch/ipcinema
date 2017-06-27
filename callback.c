#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "callback.h"
#define QUERY_SEPARATOR '^'

/*Deconcat devuelve un vector de cols elementos con los siguientes cols valores que no se consultaron
  de la querydata y avanza el cursor */
char ** deconcat(t_QueryData q, int cols){

    char ** res = malloc(cols*sizeof(char*));	
    int i, j;
    
    for(i = 0 ; i < cols ; i++){
      int value_length = 0;
      for (j = q->cursor ; q->data[j] != QUERY_SEPARATOR && q->data[j] != '\0' ; j++, value_length++)
          /* conteo */;
        
      res[i] = malloc(value_length+1);
      strncpy(res[i], q->data + q->cursor, value_length);
      
      res[i][value_length] = '\0';  //Strncpy no agrega el null termination

      q->cursor += value_length + 1;  //Avanzo el cursor hasta el siguiente valor
    }

    return res;
}

int callback(void *res, int nrCols, char **colElems, char **colsName){
  int i;
  t_QueryData result = res;
  
  result->rows++;

  char separator_str[2] = {QUERY_SEPARATOR, 0}; //Para concatenar el separador

  for (i = 0; i < nrCols ; i++){
    strcat(result->data, colElems[i] ? colElems[i] : "NULL");
    strcat(result->data, separator_str);
  }
  return 0;
}
