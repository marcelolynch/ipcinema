/*Agregar Pelicula al listado*/
#define STMNT_ADD_MOVIE "INSERT INTO Pelicula(nombre, descripcion) VALUES ('%s', '%s');"

#define STMNT_ADD_SCREENING "INSERT INTO Proyeccion(nombrePelicula, dia, mes, slot, sala) VALUES ('%s', %d, %d, %d, %d);"

#define STMNT_DELETE_SCREENING "DELETE FROM Proyeccion WHERE id = %d"

#define STMNT_DELETE_MOVIE "DELETE FROM Pelicula WHERE nombre = '%s';"
/*Realizar Reserva*/
#define STMNT_MAKE_RESERVATION "INSERT INTO RESERVA(cliente, proyeccionId, asiento) VALUES ('%s', %d, %d)"

#define STMNT_CANCEL_AND_DELETE "INSERT INTO canceladas(cliente, proyeccionId, asiento) VALUES ('%s', %d, %d); \
DELETE FROM reserva WHERE cliente = '%s' AND proyeccionId = %d AND asiento = %d;"

#define QUERY_RESERVATION_EXISTS "SELECT COUNT(*) FROM reserva WHERE cliente = '%s' AND proyeccionId = %d AND asiento = %d;"

#define QUERY_SCREENING_EXISTS "SELECT COUNT(*) FROM proyeccion WHERE id = %d;"

#define QUERY_MOVIE_EXISTS "SELECT COUNT(*) FROM pelicula WHERE nombre = '%s';"

#define QUERY_GET_SCREENINGS "SELECT id,dia,mes,slot,sala FROM Proyeccion WHERE nombrePelicula = '%s' ORDER BY mes,dia,slot,sala ASC;"

#define QUERY_GET_MOVIE_LIST "SELECT nombre,descripcion FROM Pelicula;"

#define QUERY_OCCUPIED_SEATS "SELECT asiento FROM reserva WHERE proyeccionID = %s"

#define QUERY_GET_CANCELLED "SELECT asiento, proyeccion.id, nombrePelicula, dia, mes, slot, sala \
FROM canceladas JOIN proyeccion ON proyeccion.id = canceladas.proyeccionID \
WHERE cliente = '%s' \
ORDER BY mes,dia,slot,sala ASC;"

#define QUERY_GET_RESERVED "SELECT asiento, proyeccion.id, nombrePelicula, dia, mes, slot, sala \
FROM reserva JOIN proyeccion ON proyeccion.id = reserva.proyeccionID \
WHERE cliente = '%s' \
ORDER BY mes,dia,slot,sala ASC;"

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


#define MAX_QUERY_LEN 600