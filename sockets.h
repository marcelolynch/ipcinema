typedef struct AddressCDT* Address;
typedef struct ConnectionCDT* Connection;


Connection connect_to_socket(Address addr);

Address open_socket(char* hostname, int port);

int socket_bind(Address a);

void receive_message(Connection con, char* buf);

void send_message(Connection con, char* buf);

Connection accept_connection(Address addr);