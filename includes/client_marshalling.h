typedef struct clientInstanceCDT* ClientInstance; 

ClientInstance new_client(char* srv, int portno);

int client_send(ClientInstance instance, char * msg);
int client_rcv(ClientInstance instance, char* buf);