

//MOVIDO A database.c


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