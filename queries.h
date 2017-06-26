/*Listado de Peliculas*/
#define MOVIES "SELECT nombre\
FROM Pelicula;"

/*Descripcion de Pelicula*/
#define DESCRIPTIONS "SELECT descripcion\
FROM Pelicula WHERE %s"

/*Agregar Pelicula al listado*/
#define ADDMOVIE "INSERT INTO Peliculas(nombre, descripcion)\
VALUES %s"

/*Asientos reservados*/
#define RESERVED "SELECT clienteId,nombrePelicula,proyeccionID,asiento\
FROM Reservas WHERE estado = 'Reservado';"

/*Reservas canceladas*/
#define CANCELED "SELECT clienteId,nombrePelicula,proyeccionID,asiento\
FROM Reservas WHERE estado ='Cancelado';"

/*Cancelar Reserva*/
#define CANCEL "UPDATE Reservas\
SET estado='Cancelado' WHERE %s"

/*Realizar Reserva*/
#define RESERVE "INSERT INTO Reservas(clienteId,nombrePelicula,proyeccionID,asiento,estado)\
VALUES %s"
