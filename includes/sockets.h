typedef struct AddressCDT* Address;
typedef struct ConnectionCDT* Connection;

#define BUF_SIZE 1024

Connection connect_to_socket(Address addr);

Address open_socket(char* hostname, int port);

int socket_bind(Address a);

int receive_message(Connection con, char* buf);

int send_message(Connection con, char* buf);

Connection accept_connection(Address addr);