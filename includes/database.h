#ifndef __db_h
#define __db_h

typedef struct databasesessionCDT* DbSession;
typedef struct querydataCDT * QueryData;


DbSession db_init();

#define DB_ERR_CONSTRAINT -1
#define DB_ERR_OTHER	  -2


int do_query(DbSession session, QueryData qdata);
int execute_statements(DbSession session, char* statements);

void db_close(DbSession s);

QueryData new_query(char* query, int cols);
void destroy_query_data(QueryData q);
char ** next_row(QueryData q);

//Devuelve la cantidad de columnas 
int n_cols(QueryData q);

//Devuelve la cantidad de filas
int n_rows(QueryData q);


#endif