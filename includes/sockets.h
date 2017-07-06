#ifndef __sockts_h
#define __sockts_h

#include <stdlib.h>

typedef struct AddressCDT* Address;
typedef struct ConnectionCDT* Connection;

Connection connect_to_socket(Address addr);

Address open_socket(char* hostname, int port);

int socket_bind(Address a);

int receive_message(Connection con, char* buf, size_t len);

int send_message(Connection con, char* buf, size_t len);

Connection accept_connection(Address addr);
void destroy_connection(Connection c);

#endif