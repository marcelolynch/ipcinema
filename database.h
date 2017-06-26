#define TABLESCREATE "CREATE TABLE IF NOT EXISTS Pelicula(\
nombre 	TEXT,\
descripcion 	TEXT,\
PRIMARY KEY(nombre)\
);\
CREATE TABLE IF NOT EXISTS Proyeccion\
(\
id INTEGER PRIMARY KEY AUTOINCREMENT,\
nombrePelicula 	TEXT,\
dia 	INT CHECK(dia BETWEEN 1 AND 7),\
slot 	INT CHECK(slot BETWEEN 1 AND 8),\
sala	INTEGER CHECK(sala BETWEEN 1 and 10),
UNIQUE(dia,slot, sala),\
FOREIGN KEY(nombrePelicula) REFERENCES Pelicula ON DELETE CASCADE ON UPDATE RESTRICT\
);\
\
CREATE TABLE IF NOT EXISTS Reservas\
(\
clienteId INT,\
nombrePelicula TEXT,\
proyeccionID INT,\
asiento INT,\
estado  TEXT,\
PRIMARY KEY(proyeccionID, asiento),\
FOREIGN KEY(clienteId) REFERENCES Cliente ON DELETE CASCADE ON UPDATE RESTRICT,\
FOREIGN KEY(proyeccionID) REFERENCES Proyeccion ON DELETE SET NULL ON UPDATE RESTRICT,\
FOREIGN KEY(nombrePelicula) REFERENCES Pelicula ON DELETE CASCADE ON UPDATE RESTRICT\
);\
\
CREATE TABLE IF NOT EXISTS Cliente\
(\
id 	INT,\
dni INT,\
nombre 	TEXT,\
apellido 	TEXT,\
direccion TEXT,\
telefono  TEXT,\
PRIMARY KEY(ID),\
UNIQUE(dni)\
);"
