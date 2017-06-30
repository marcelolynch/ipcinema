#include "sqlite3.h"
#include "callback.h"
#include "db.h"
#include "utilities.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "callback.h"
#include "server_logging.h"

#define TABLES_CREATE "CREATE TABLE IF NOT EXISTS Pelicula(\
nombre  TEXT,\
descripcion   TEXT,\
PRIMARY KEY(nombre)\
);\
CREATE TABLE IF NOT EXISTS Proyeccion\
(\
id INTEGER PRIMARY KEY AUTOINCREMENT,\
nombrePelicula  TEXT,\
dia   INT CHECK(dia BETWEEN 1 AND 31),\
mes   INT CHECK(mes BETWEEN 1 and 12),\
slot  INT CHECK(slot > 1),\
sala  INTEGER CHECK(sala > 1),\
UNIQUE(dia,slot, sala),\
FOREIGN KEY(nombrePelicula) REFERENCES Pelicula(nombre) ON DELETE CASCADE ON UPDATE RESTRICT\
);\
\
CREATE TABLE IF NOT EXISTS Reserva\
(\
cliente TEXT,\
proyeccionID INTEGER,\
asiento INTEGER,\
estado  TEXT,\
PRIMARY KEY(proyeccionID, asiento),\
FOREIGN KEY(proyeccionID) REFERENCES Proyeccion(id) ON DELETE CASCADE ON UPDATE RESTRICT\
);"

// En reservas:
//cliId INTEGER,
//FOREIGN KEY(cliId) REFERENCES Cliente ON DELETE CASCADE ON UPDATE RESTRICT,


#define QUERY_SEPARATOR '^'
#define INITIAL_CAPACITY 10

#define DATABASE_FILENAME "cine.db"

struct databasesessionCDT {
	sqlite3 * db;
	char *err_msg;
};

struct querydataCDT {

  // Se define en la inicializacion
  char * query;
  
  // Se define en la inicializacion,
  // deberia ser consistente con la query         
  int cols;
  
  // Aumenta por cada callback de SQLite           
  int rows;
              
  // Apunta a la proxima fila a consultar (una vez que se empiezan
  // a consultar los datos con next_row)
  int cursor;

  // Matriz de strings: guarda los resultados de la query (NULL si el valor es NULL en la base de datos)
  char* ** data;
              
  // Para manejar la memoria reservada de data (rows <= capacity en todo momento; 
  //data tiene reservado capacity*sizeof(char* *)) bytes  
  size_t capacity;
  };


static int callback(void *res, int nrCols, char **colElems, char **colsName);



DbSession db_init(){
	DbSession session = failfast_malloc(sizeof(*session));

    int rc = sqlite3_open(DATABASE_FILENAME, &session->db);
    
    sqlite3_exec(session->db, "PRAGMA foreign_keys = ON", NULL, NULL, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(session->db));
        sqlite3_close(session->db);
        free(session);
        return NULL;
    }

    rc = sqlite3_exec(session->db, TABLES_CREATE, NULL, NULL, &session->err_msg);

    if (rc != SQLITE_OK ) {
        
        printf("[ERRROR] Failed to create database tables.\n");
        printf("[ERROR] SQL error: %s\n", session->err_msg);
        sqlite3_free(session->err_msg);
        free(session);
        return NULL;

    } else {
        printf("Database Cinema created successfully!\n");
    	return session;
    }

    
}


// Rellena la QueryData con el resultado de la query
// QueryData tiene que haber sido inicializada antes y ser
// consistente con la query
int do_query(DbSession session, QueryData qdata){
	int rc = sqlite3_exec(session->db, qdata->query, callback, qdata, &session->err_msg);

    if (rc != SQLITE_OK ) {
      srv_log(session->err_msg);
		  return rc;    	
    }

    srv_log("Succesful query");
    return 1;
}


// Ejecuta sentencias SQL sin esperar una respuesta de la base de datos
// TODO: TAD para statements? Manejo de errores
int execute_statements(DbSession session, char* statements){

	int rc = sqlite3_exec(session->db, statements, NULL, NULL, &session->err_msg);

    if (rc != SQLITE_OK ) {
      srv_log(session->err_msg);
		  if(rc == SQLITE_CONSTRAINT){
        return DB_ERR_CONSTRAINT;
      } else {
        return DB_ERR_OTHER;
      }

    }
  
    srv_log("Succesful query");
    return 1;

}



void db_close(DbSession s){
   sqlite3_close(s->db);
   free(s);
}


// Lo que sigue son funciones correspondientes al TAD de QueryData
static int increase_capacity(QueryData q);

static int callback(void *res, int nrCols, char **colElems, char **colsName){
  int i;
  QueryData q = res;

  if(nrCols != q->cols){
    printf("Consistency error! Should never happen\n");
  }

  // q->rows apunta al indice de la siguiente fila a llenar
  if(q->rows >= q->capacity - 1){
    increase_capacity(q); //TODO: Check null
  }

  // Reservo espacio para una fila mas
  q->data[q->rows] = failfast_malloc(q->cols * sizeof(char*)); 

  for (i = 0; i < nrCols ; i++){
    if(colElems[i] == NULL){
      q->data[q->rows][i] = NULL;
    } 
    else {
      q->data[q->rows][i] = failfast_malloc(strlen(colElems[i]) + 1);
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



QueryData new_query(char* query, int cols){
  QueryData q = failfast_malloc(sizeof(*q));
  if(q == NULL){
      return q;
  }

  q->query = failfast_malloc(strlen(query) + 1);
  strcpy(q->query, query);

  q->cursor = 0;
  q->rows = 0;
  q->cols = cols;
  q->data = failfast_malloc(INITIAL_CAPACITY * sizeof(char* *));
  q->capacity = INITIAL_CAPACITY;

  return q;
}

//Devuelve la cantidad de columnas 
int n_cols(QueryData q){
  return q->cols;
}


//Devuelve la cantidad de filas
int n_rows(QueryData q){
  return q->rows;
}


static int increase_capacity(QueryData q){
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
  char ** res = failfast_malloc(q->cols*sizeof(char*)); 
  int i;
  for(i = 0 ; i < q->cols ; i++){
    res[i] = q->data[q->cursor][i];
  }

  q->cursor++;
  return res;
}

