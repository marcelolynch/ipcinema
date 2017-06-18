#include <sqlite3.h>
#include <stdio.h>

int main(void) {
    
    sqlite3 *database;
    char *err_msg = 0;
    
    /* Open the file with the database and make reference to the one passed as second parameter. Returns the result of the operation. */
    int rc = sqlite3_open("/home/favarela/database.db" , &database);
    
    if (rc != SQLITE_OK) {        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(database));
        sqlite3_close(database);
        return 1;
    }
    
    /* SQL String that will be executed next.*/
    
    char* sql = 
    "CREATE TABLE Cinema(Nro INTEGER PRIMARY KEY, Asientos INTEGER, Asientos_Disp INTEGER, Asientos_Ocup INTEGER,Detalles TEXT);"
    "INSERT INTO Cinema VALUES (1,50,10,0,'3D');"
    "INSERT INTO Cinema VALUES (2,25,25,0,'2D');"
    "INSERT INTO Cinema VALUES (3,40,40,0,'2D');"
    "INSERT INTO Cinema VALUES (4,30,30,0,'2D');"
    "INSERT INTO Cinema VALUES (5,100,100,0,'IMAX');";
    
    /*The sqlite3_exec() function is a convenience wrapper that allows an application to run multiple statements of SQL without having to use a lot of C code.*/
    
    rc = sqlite3_exec(datab, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to create table\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        
    } else {
        
        fprintf(stdout, "Table Cinema created successfully!\n");
    }

char *sql_2 = "SELECT * FROM Cinema";
        
    rc = sqlite3_exec(db, sql_2, callback, 0, &err_msg);
    
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
