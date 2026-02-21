#ifndef SERVER_H
#define SERVER_H

#include <windows.h>

int server_init(SOCKET *resultSocket);
int server_tick(SOCKET listenSocket);
int enqueue(SOCKET socket, char *buffer, size_t buffer_len);

#endif