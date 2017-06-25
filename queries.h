/*Listado de Peliculas*/
SELECT nombre
FROM Pelicula

/*Descripcion de Pelicula*/
SELECT descripcion
FROM Pelicula
WHERE nombre= ...

/*Agregar Pelicula al listado*/
INSERT INTO Peliculas(nombre, descripcion)
values ...

/*Asientos reservados*/
SELECT cliente, nombrePelicula, dia, slot, asiento
FROM Reservas
WHERE estado="Reservado"

/*Reservas canceladas*/
SELECT cliente, nombrePelicula, dia, slot, asiento
FROM Reservas
WHERE estado="Cancelado"

/*Reservas canceladas*/
SELECT cliente, nombrePelicula, dia, slot, asiento
FROM Reservas
WHERE estado="Cancelado"

/*Cancelar Reserva*/
UPDATE Reservas
SET estado="Cancelado"
WHERE nombrePelicula= ... , dia= ... , slot= ... ,asiento= ...

/*Realizar Reserva*/
INSERT INTO Reservas(clienteId,nombrePelicula,dia,slot,asiento)
VALUES ...
