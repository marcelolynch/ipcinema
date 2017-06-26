/*deconcat te devuelve el primer elemento del string concatenado y lo elimina del string*/
char *deconcat(char *info){
    for (int i= 0 ; info[i] !='|' && info[i] != '\0' ; i++);
    strncpy(char* res, info, i);
    i++;
    for (int j = 0 ; info[j] != '\0' ; j++ )
    info[j]= info[j+i];
    info[j]='\0';
    return res;
}

int callback(void *res, int nrCols, char **colElems, char **colsName){

  struc Datos *result = res;
  result->rows = result->rows++;

  if (strlen(result->data)==0){
    strcopy(result->data,colElems[0]);
  for (int i = 0; i < nrCols ; i++)
    strcat(result->data,"|");
    strcat(result->data,colElems[i]);
  }
  return 0;
}
