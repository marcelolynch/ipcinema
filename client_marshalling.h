typedef struct clientInstanceCDT* ClientInstance; 

ClientInstance new_client(char* srv);

void client_send(ClientInstance instance, char * msg);