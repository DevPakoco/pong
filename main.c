#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // For sleep()
#include "common/packets.h"
#include "server/server.h"
#include "client/client.h"


void* run_server(void* arg) {
    SOCKET listen_socket;
    server_init(&listen_socket);
    for (;;) {
        int result = server_tick(listen_socket);
        if (result != 0) {
            printf("server tick error, breaking loop\n");
            break;
        }
    }
    return NULL;
}

void* run_client(void* arg) {
    char* ip = (const char*)arg;
    SOCKET client_socket;
    client_init(ip, &client_socket);
    Sleep(1000);
    for (;;) client_tick(&client_socket);
    return NULL;
}

int main() {
    pthread_t server_thread, client_thread;
    if (pthread_create(&server_thread, NULL, run_server, NULL) != 0) {
        perror("failed to create server thread");
        return 1;
    }

    sleep(1); // Give the server a moment to start listening (adjust sleep time if needed)
    if (pthread_create(&client_thread, NULL, run_client, "127.0.0.1") != 0) {
        perror("failed to create client thread");
        return 1;
    }
    getchar(); // Pause the main thread until Enter is pressed (threads continue running in the background)
    return 0;
}
