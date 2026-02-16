#ifndef SERVER_H
#define SERVER_H
#include <windows.h>

int InitServer(SOCKET *resultSocket);
int ConnectionAccept(SOCKET *listenSocket, SOCKET *clientSocket);
int EchoLoop(SOCKET *clientSocket);
int ShutdownConnection(SOCKET *socket);
int server_test();

#endif