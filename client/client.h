#ifndef CLIENT_H
#define CLIENT_H

int client_init(char *server_addr, SOCKET *result_socket);
int client_tick(SOCKET *clientSocket);

#endif