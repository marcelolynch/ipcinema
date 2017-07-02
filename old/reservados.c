/*Para el caso de asientos reservados*/

#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include "queries.h"
#include "database.h"
#include "callback.h"

int main(void) {

    sqlite3 *database;
    char *err_msg = 0;

    int rc = sqlite3_open("database.db" , &database);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(database));
        sqlite3_close(database);
        return 1;
    }

    rc = sqlite3_exec(database, TABLESCREATE, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to create database tables.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Database Cinema created successfully!\n");
    }

    char* query = "INSERT INTO Pelicula VALUES ('Matrix', 'Matrix la pelicula' );"
                  "INSERT INTO Pelicula VALUES ('Harry Potter', 'La piedra filosofal' );"
                  "INSERT INTO Pelicula VALUES ('El senor de los Anillos', 'La primera' );"
                  "INSERT INTO Proyeccion(nombrePelicula, dia, slot, sala) VALUES ('Matrix', 2, 3, 4);"
                  "INSERT INTO Proyeccion(nombrePelicula, dia, slot, sala) VALUES ('Harry Potter', 2, 4, 4);";

    rc = sqlite3_exec(database, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to insert movies.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Inserted movies\n");
    }

    char *query2= "INSERT INTO Cliente VALUES (0,300,'Pepe','Gomez','Calle Falsa 1234','4700-1122');";

    rc = sqlite3_exec(database, query2, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to insert client.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Inserted client.\n");
    }

    char *query3 = "(0,'Matrix',0,4,'Reservado'), (1,'Matrix',NULL,5,'Reservado'),  (1,'Matrix',NULL,5,'Reservado'),  (1,'Matrix',NULL,5,'Reservado'),  (1,'Matrix',NULL,5,'Reservado')";
    char *query4 = sqlite3_mprintf (RESERVE, query3);
    
    printf("%s\n", query4);

    rc = sqlite3_exec (database, query4, 0, 0, &err_msg);
    sqlite3_free (query4);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to reserve the seat.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Inserted reserve.\n");
    }


    QueryData qdata = new_query_data(4);
    printf("%s\n", RESERVED);
    rc = sqlite3_exec(database, RESERVED, callback, qdata, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to consult table.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Reserved Seats: %d\n", n_rows(qdata));
        fprintf(stdout, "Client Id - Movie - Projection Id - Seat");
        char ** row_values;
        while((row_values = next_row(qdata)) != NULL){
             printf("\n");
            for (int j = 0 ; j < 4 ; j++){
             printf("%s  ", row_values[j] ? row_values[j] : "NULL");
            }
        }
        
    }

    sqlite3_close(database);
    return 0;
}
