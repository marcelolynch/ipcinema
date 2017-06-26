/*Para el caso de asientos reservados*/

#include "sqlite3.h"
#include <stdio.h>
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

    char* query = "INSERT INTO Pelicula(nombre, descripcion) VALUES ('Matrix', 'La pelicula matrix' );"
                  "INSERT INTO Pelicula(nombre, descripcion) VALUES ('Matrix Recargado', 'La pelicula matrix' );"
                  "INSERT INTO Pelicula(nombre, descripcion) VALUES ('Matrix Revoluciones', 'La pelicula matrix' );"
                  "INSERT INTO Proyeccion(nombrePelicula, dia, slot) VALUES ('Matrix', 2, 3);"
                  "INSERT INTO Proyeccion(nombrePelicula, dia, slot) VALUES ('Matrix Recargado', 2, 3);";

    rc = sqlite3_exec(database, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to insert movies.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Inserted movies\n");
    }

    char *query2= "INSERT INTO Cliente(0,30000000,\'Pepe\',\'Gomez\',\'Calle Falsa 1234\',\'4700-1122\');";

    rc = sqlite3_exec(database, query2, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to insert client.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Inserted client.\n");
    }

    rc = sqlite3_exec_printf(database, RESERVE, 0, 0, &err_msg,"0,\'Matrix',0,4,\'Reservado\'");

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to reserve the seat.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Inserted reserve.\n");
    }

    struct Datos tupla;

    rc = sqlite3_exec(database, RESERVED, callback, &tupla, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to consult table.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "Reserved Seats: %d\n",tupla.filas);
        fprintf(stdout, "Client Id - Movie - Projection Id - Seat");
        for (int i = 0 ; i < tupla.filas ; i++){
          for (int j= 0 ; j < 4){
            fprintf(stdout,deconcat(tupla.data));
          }
        }
    }

    sqlite3_close(database);
    return 0;
}
