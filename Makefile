CC = gcc
CFLAGS = -Wall -I $(INCLUDE_DIR)
DEBUGFLAG = -D DEBUG

INCLUDE_DIR = ./includes
COMMON_SOURCES = sockets.c errors.c utilities.c
SERVER_SOURCES = server.c server_marshalling.c server_logging.c request_constructor.c synchronization.c database.c sqlite3.c
SERVER_FLAGS = -pthread -ldl
CLIENT_SOURCES = client.c ui.c client_marshalling.c 
CLIENT_DEBUG = client_debug.c client_marshalling.c 

LOG_SOURCES = logger.c

SERVER_BINARY = server.bin
CLIENT_BINARY = client.bin
LOG_BINARY = logger.bin

all: client server log

debug: clientdebug serverdebug log

client: $(CLIENT_SOURCES) $(COMMON_SOURCES)
	$(CC) $(CFLAGS) $(COMMON_SOURCES) $(CLIENT_SOURCES) -o $(CLIENT_BINARY)

clientdebug: $(CLIENT_DEBUG) $(COMMON_SOURCES)
	$(CC) $(CFLAGS) $(DEBUGFLAG) $(COMMON_SOURCES) $(CLIENT_DEBUG) -o $(CLIENT_BINARY)

server: $(SERVER_SOURCES) $(COMMON_SOURCES)
	$(CC) $(CFLAGS) $(COMMON_SOURCES) $(SERVER_SOURCES) $(SERVER_FLAGS) -o $(SERVER_BINARY)

serverdebug: $(SERVER_SOURCES) $(COMMON_SOURCES)
	$(CC) $(CFLAGS) $(DEBUGFLAG) $(COMMON_SOURCES) $(SERVER_SOURCES) $(SERVER_FLAGS) -o $(SERVER_BINARY)

log: $(LOG_SOURCES)
	$(CC) $(CFLAGS) $(LOG_SOURCES) -pthread -o $(LOG_BINARY) 


.PHONY: clean

clean:
	rm *.bin