# IPCinema

## Compilación
`make all` dentro del directorio raíz genera los ejecutables *client.bin*, *server.bin* y *logger.bin*, correspondientes al proceso cliente, servidor y el daemon de logging.

Con `make clean` se borran los binarios.

## Ejecución
### Servidor
El servidor se corre invocando `./server.bin <port>`, donde `<port>` es el numero de puerto donde se escucharán las conexiones entrantes. 

### Cliente
Para ejecutar el cliente se invoca `./client.bin <hostname> <port>`, donde `<hostname>` es la direccion del host del servidor (o `localhost` desde la máquina local) y `<port>`el puerto de escucha (coincidente con el que se indicó al levantar el servidor).