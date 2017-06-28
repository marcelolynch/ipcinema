#ifndef __srv_mrsh_h
#define __srv_mrsh_h


typedef struct serverinstanceCDT* ServerInstance;
typedef struct clientsessionCDT* ClientSession;


ClientRequest * wait_request(ClientSession cli);
ClientSession wait_client(ServerInstance srv);

ServerInstance server_init(int port);

int send_screenings(ClientSession session, ScreeningDataList* screenings);
int send_movies(ClientSession session, MovieInfoList* movies);
int send_seats(ClientSession session, char* seats);

void client_send_error(ClientSession cli);
void client_send_ok(ClientSession cli);

#endif