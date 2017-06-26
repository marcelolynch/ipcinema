/*Listado de Peliculas*/
#define MOVIES "SELECT nombre\
FROM Pelicula"

/*Descripcion de Pelicula*/
#define DESCRIPTIONS "SELECT descripcion\
FROM Pelicula\
WHERE %S"

/*Agregar Pelicula al listado*/
#define ADDMOVIE "INSERT INTO Peliculas(nombre, descripcion)\
values %S"

/*Asientos reservados*/
#define RESERVED "SELECT cliente, nombrePelicula, dia, slot, asiento\
FROM Reservas\
WHERE estado=\"Reservado\""

/*Reservas canceladas*/
#define CANCELED "SELECT cliente, nombrePelicula, dia, slot, asiento\
FROM Reservas\
WHERE estado=\"Cancelado\""

/*Cancelar Reserva*/
#define CANCEL "UPDATE Reservas\
SET estado=\"Cancelado\"\
WHERE %S"

/*Realizar Reserva*/
#define RESERVE "INSERT INTO Reservas(clienteId,nombrePelicula,dia,slot,asiento)\
VALUES %S"
