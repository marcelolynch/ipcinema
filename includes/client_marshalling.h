typedef struct clientInstanceCDT* ClientInstance; 

ClientInstance new_client(char* srv, int portno);

void client_send(ClientInstance instance, char * msg);
void client_rcv(ClientInstance instance, char* buf);