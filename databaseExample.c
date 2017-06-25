#include "sqlite3.h"
#include <stdio.h>
#include "database.h"
int callback(void *NotUsed, int argc, char **argv, char **azColName);

int main(void) {
    
    sqlite3 *database;
    char *err_msg = 0;
    
    /* Open the file with the database and make reference to the one passed as second parameter. Returns the result of the operation. */
    int rc = sqlite3_open("database.db" , &database);
    
    if (rc != SQLITE_OK) {        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(database));
        sqlite3_close(database);
        return 1;
    }
    
    /* SQL String that will be executed next.*/
    
 
    /*The sqlite3_exec() function is a convenience wrapper that allows an application to run multiple statements of SQL without having to use a lot of C code.*/
    
    rc = sqlite3_exec(database, TABLESCREATE, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to create table\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        
    } else {
        
        fprintf(stdout, "Table Cinema created successfully!\n");
    }

char* query = "INSERT INTO Pelicula(nombre, descripcion) VALUES ('Matrix', 'La pelicula matrix' );"
              "INSERT INTO Pelicula(nombre, descripcion) VALUES ('Matrix Recargado', 'La pelicula matrix' );"
              "INSERT INTO Pelicula(nombre, descripcion) VALUES ('Matrix Revoluciones', 'La pelicula matrix' );"
              "INSERT INTO Proyeccion(nombrePelicula, dia, slot) VALUES ('Matrix', 2, 3);"
              "INSERT INTO Proyeccion(nombrePelicula, dia, slot) VALUES ('Matrix Recargado', 2, 3);";

rc = sqlite3_exec(database, query, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to create table\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        
    } else {
        
        fprintf(stdout, "Inserted movies\n");
    }


char *sql_2 = "SELECT * FROM Proyeccion";
        
    rc = sqlite3_exec(database, sql_2, callback, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(database);
        return 1;
    } 

    sqlite3_close(database);
    return 0;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}
