#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "../common/common.h"
#include "../common/packets.h"
#include "server.h"
#include "workflow.h"

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define MAX_CLIENTS 100

static Linfo clients = {0};
static const u_long FIONBIO_MODE = 1;


int server_init(SOCKET *resultSocket) {
    printf("server_init\n");
    WSADATA wsaData;
    SOCKET connSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    connSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (connSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    iResult = bind(connSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(connSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);
    iResult = listen(connSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(connSocket);
        WSACleanup();
        return 1;
    }
    ioctlsocket(connSocket, FIONBIO, &FIONBIO_MODE);
    *resultSocket = connSocket;
    printf("server_init done\n");
    return 0;
}

int connection_shutdown(SOCKET socket) {
    printf("connection_shutdown\n");
    int iResult = shutdown(socket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        return 1;
    }
    closesocket(socket);
    printf("connection_shutdown done\n");
    return 0;
}

int connection_accept(SOCKET listenSocket, SOCKET *clientSocket) {
    *clientSocket = accept(listenSocket, NULL, NULL);
    if (*clientSocket == INVALID_SOCKET) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return -1;
        }
        return 1;
    }
    return 0;
}

void new_client_init(SOCKET client_socket) {
    printf("init_client\n");
    client_t *client = calloc(1, sizeof(*client));
    Node *nod = head(&clients);
    client->session_id = rand();
    client->socket = client_socket;
    client->workflow_state = (workflow_state_t){
        .workflow = WORKFLOW_LOGIN,
        .login_state = LOGIN_UNLOGGED
    };
    nod->pdata = client;
}

int new_connections(SOCKET listenSocket) {
    SOCKET client_socket;
    int result;
    while (1) {
        result = connection_accept(listenSocket, &client_socket);
        if (result == 0) new_client_init(client_socket);
        else if (result == 1) break;
        else return 1;
    }
    return 0;
}

int server_tick(SOCKET listen_socket) {
    if (new_connections(listen_socket) != 0) return 1;
    Node *nod = clients.head;
    while (nod != NULL) {
        client_t *client = nod->pdata;
        if (client_status_update(client) != 0) printf("error updating client status\n");
        nod = nod->next;
    }
    Sleep(500);
    return 0;
}

int enqueue(SOCKET socket, char *buffer, size_t buffer_len) {
    int result = send(socket, buffer, buffer_len, 0);
    if (result == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        connection_shutdown(socket);
        return 1;
    }
    printf("bytes sent: %d\n", result);
    return 0;
}





/*int EchoLoop(SOCKET *clientSocket) {
    printf("EchoLoop...\n");
    int iResult, iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    // Receive until the peer shuts down the connection
    do
    {
        iResult = recv(*clientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            // Echo the buffer back to the sender
            iSendResult = send(*clientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(*clientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0) printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(*clientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);
    printf("EchoLoop done\n");
    return 0;
}*/