
/*Listado de Peliculas*/
#define MOVIES "SELECT nombre \
FROM Pelicula;"

/*Descripcion de Pelicula*/
#define DESCRIPTIONS "SELECT descripcion \
FROM Pelicula WHERE %s"

/*Agregar Pelicula al listado*/
#define ADD_MOVIE "INSERT INTO Pelicula(nombre, descripcion) VALUES ('%s', '%s');"

#define ADD_SCREENING "INSERT INTO Proyeccion(nombrePelicula, dia, slot, sala) VALUES ('%s', %d, %d, %d);"

#define DELETE_SCREENING "DELETE FROM Proyeccion WHERE dia = %d AND slot = %d AND sala = %d;"

#define DELETE_MOVIE "DELETE FROM Pelicula WHERE nombre = '%s';"

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