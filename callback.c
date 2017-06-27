#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "callback.h"

#define QUERY_SEPARATOR '^'
#define INITIAL_CAPACITY 10

QueryData new_query_data(int cols){
  QueryData q = malloc(sizeof(*q));
  if(q == NULL){
      return q;
  }
  q->cursor = 0;
  q->rows = 0;
  q->cols = cols;
  q->data = malloc(INITIAL_CAPACITY * sizeof(char* *));
  q->capacity = INITIAL_CAPACITY;
}


int increase_capacity(QueryData q){
  if(q == NULL) return -1;

  char* **newdata = realloc(q->data, 2*q->capacity*sizeof(char**));
  
  if(newdata == NULL){
    //No memory
    return -1;
  }
  
  q->data = newdata;
  q->capacity = 2*q->capacity;

  return 1;
}

/* next_row devuelve un vector de cols elementos con la siguiente fila que no se consulto
  de la querydata y avanza el cursor.
  Importante: se retorna un shallow copy de la fila: no hay que modificar los valores
  ni hacer free desde afuera  */
char ** next_row(QueryData q){
  if(q->cursor >= q->rows){
    return NULL;  //No existe una siguiente
  }
  char ** res = malloc(q->cols*sizeof(char*)); 
  int i;
  for(i = 0 ; i < q->cols ; i++){
    res[i] = q->data[q->cursor][i];
  }

  q->cursor++;
  return res;
}

int callback(void *res, int nrCols, char **colElems, char **colsName){
  int i;
  QueryData q = res;

  if(nrCols != q->cols){
    printf("Consistency error! Should never happen\n");
  }

  // q->rows apunta al indice de la siguiente fila a llenar
  if(q->rows >= q->capacity - 1){
    increase_capacity(q); //TODO: Check null
  }

  q->data[q->rows] = malloc(q->cols * sizeof(char*)); 

  for (i = 0; i < nrCols ; i++){
    if(colElems[i] == NULL){
      q->data[q->rows][i] = NULL;
    } 
    else {
      q->data[q->rows][i] = malloc(strlen(colElems[i]) + 1);
      strcpy(q->data[q->rows][i], colElems[i]);
    }
  }

  q->rows++;
  return 0;
}


void destroy_query_data(QueryData q){
  if(q == NULL){
    return;
  }
  int i, j;

  for(i = 0; i < q->rows ; i++){
    for(j = 0 ; j < q->cols ; j++){
    
      //Libero los strings
      free(q->data[i][j]);
    }
    //Libero el vector a los strings
    free(q->data[i]);
  }

  //Libero la matriz data (vector de vectores a strings)
  free(q->data);

  //Libero el espacio para la estructura
  free(q);
}

/*Deconcat devuelve un vector de cols elementos con los siguientes cols valores que no se consultaron
  de la querydata y avanza el cursor 
char ** deconcat_old(QueryData q, int cols){

    char ** res = malloc(cols*sizeof(char*)); 
    int i, j;
    
    for(i = 0 ; i < cols ; i++){
      int value_length = 0;
      
      j = q->cursor;
      while(q->data[j] != QUERY_SEPARATOR && q->data[j] != '\0'){
          j++;
          value_length++; //Cuento caracteres
      }
        
      res[i] = malloc(value_length+1);
      strncpy(res[i], q->data + q->cursor, value_length);      
      res[i][value_length] = '\0';  //Strncpy no agrega el null termination

      q->cursor += value_length + 1;  //Avanzo el cursor hasta el siguiente valor
    }

    return res;
}



int callback_old(void *res, int nrCols, char **colElems, char **colsName){
  int i;
  QueryData result = res;
  
  result->rows++;

  for (i = 0; i < nrCols ; i++){
    strcat(result->data, colElems[i] ? colElems[i] : "NULL");
    strcat(result->data, "|");
  }
  return 0;
}
*/