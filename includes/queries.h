/*Agregar Pelicula al listado*/
#define STMNT_ADD_MOVIE "INSERT INTO Pelicula(nombre, descripcion) VALUES ('%s', '%s');"

#define STMNT_ADD_SCREENING "INSERT INTO Proyeccion(nombrePelicula, dia, mes, slot, sala) VALUES ('%s', %d, %d, %d, %d);"

#define STMNT_DELETE_SCREENING "DELETE FROM Proyeccion WHERE dia = %d AND month = %d AND slot = %d AND sala = %d;"

#define STMNT_DELETE_MOVIE "DELETE FROM Pelicula WHERE nombre = '%s';"
/*Realizar Reserva*/
#define STMNT_MAKE_RESERVATION "INSERT INTO RESERVA(cliente, proyeccionId, asiento, estado) VALUES ('%s', %d, %d, 'Reservado')"

#define QUERY_GET_SCREENINGS "SELECT id,dia,mes,slot,sala FROM Proyeccion WHERE nombrePelicula = '%s' ORDER BY mes,dia,slot,sala ASC;"

#define QUERY_GET_MOVIE_LIST "SELECT nombre,descripcion FROM Pelicula;"

#define QUERY_OCCUPIED_SEATS "SELECT asiento FROM reserva WHERE proyeccionID = %s"

/*Asientos reservados*/
#define RESERVED "SELECT cliId , nombrePelicula , proyeccionID,asiento \
FROM Reservas WHERE estado = 'Reservado';"

/*Reservas canceladas*/
#define CANCELED "SELECT cliId,nombrePelicula,proyeccionID,asiento \
FROM Reserva WHERE estado ='Cancelado';"

/*Cancelar Reserva*/
#define CANCEL "UPDATE Reservas\
SET estado='Cancelado' WHERE %s"

/*Realizar Reserva*/
#define ADD_RESERVATION "INSERT INTO Reserva(cliente,proyeccionID,asiento,estado) \
VALUES ('%s', ;"


#define QUERY_LEN_OVERHEAD 100