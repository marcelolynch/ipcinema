CC = gcc
CFLAGS = -Wall -pedantic -g -I $(INCLUDE_DIR)

INCLUDE_DIR = ./includes
COMMON_SOURCES = sockets.c utilities.c
SERVER_SOURCES = server.c server_marshalling.c server_logging.c request_builder.c synchronization.c database.c sqlite3.c
SERVER_FLAGS = -pthread -ldl
CLIENT_SOURCES = client.c ui.c client_marshalling.c 

LOG_SOURCES = logger.c

SERVER_BINARY = server.bin
CLIENT_BINARY = client.bin
LOG_BINARY = logger.bin

all: client server log

client: $(CLIENT_SOURCES) $(COMMON_SOURCES)
	$(CC) $(CFLAGS) $(COMMON_SOURCES) $(CLIENT_SOURCES) -o $(CLIENT_BINARY)

server: $(SERVER_SOURCES) $(COMMON_SOURCES)
	$(CC) $(CFLAGS) $(COMMON_SOURCES) $(SERVER_SOURCES) $(SERVER_FLAGS) -o $(SERVER_BINARY)

log: $(LOG_SOURCES)
	$(CC) $(CFLAGS) $(LOG_SOURCES) -pthread -o $(LOG_BINARY) 


.PHONY: clean

clean:
	rm *.bin