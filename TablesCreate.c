#DEFINE TABLESCREATE "
CREATE TABLE IF NOT EXISTS Pelicula
(
nombre 	TEXT,
descripcion 	TEXT,
PRIMARY KEY(nombre)
);

CREATE TABLE IF NOT EXISTS Proyeccion
(
nombrePelicula 	TEXT,
dia 	DATE,
slot 	INT,
PRIMARY KEY(dia,slot),
FOREIGN KEY(nombrePelicula) REFERENCES Pelicula ON DELETE CASCADE ON UPDATE RESTRICT
);

CREATE TABLE IF NOT EXISTS Reservas
(
clienteId INT,
nombrePelicula TEXT,
dia 	date,
slot 	INT,
asiento TEXT,
estado  TEXT,
PRIMARY KEY(dia, slot, asiento)
FOREIGN KEY(clienteId) REFERENCES Cliente ON DELETE CASCADE ON UPDATE RESTRICT
FOREIGN KEY(nombrePelicula) REFERENCES Pelicula ON DELETE CASCADE ON UPDATE RESTRICT
);

CREATE TABLE IF NOT EXISTS Cliente
(
id 	INT,
dni INT,
nombre 	TEXT,
apellido 	TEXT,
direccion TEXT,
telefono  TEXT,
PRIMARY KEY(ID, dni),
);
"
