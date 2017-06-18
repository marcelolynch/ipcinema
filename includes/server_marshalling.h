#ifndef __srv_mrsh_h
#define __srv_mrsh_h


typedef struct serverinstanceCDT* ServerInstance;
typedef struct clientsessionCDT* ClientSession;


ClientRequest * wait_request(ClientSession cli);
ClientSession wait_client(ServerInstance srv);

ServerInstance server_init(int port);

void client_send(ClientSession client, char* msg);

#endif