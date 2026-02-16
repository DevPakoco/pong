#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // For sleep()
#include "server/server.h"
#include "client/client.h"

void* run_server(void* arg) {
    SOCKET listenSocket, clientSocket;
    InitServer(&listenSocket);
    ConnectionAccept(&listenSocket, &clientSocket);
    EchoLoop(&clientSocket);
    ShutdownConnection(&clientSocket);
    //server_test();
    return NULL;
}

void* run_client(void* arg) {
    const char* ip = (const char*)arg;
    client_test(ip); // connects to the server and exchanges messages...
    return NULL;
}

int main() {
    pthread_t server_thread, client_thread;
    if (pthread_create(&server_thread, NULL, run_server, NULL) != 0) {
        perror("Failed to create server thread");
        return 1;
    }

    sleep(1); // Give the server a moment to start listening (adjust sleep time if needed)
    if (pthread_create(&client_thread, NULL, run_client, "127.0.0.1") != 0) {
        perror("Failed to create client thread");
        return 1;
    }
    getchar(); // Pause the main thread until Enter is pressed (threads continue running in the background)
    return 0;
}