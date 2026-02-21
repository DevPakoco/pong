#include <stdio.h>
#include "workflow.h"
#include "server.h"
#include "../common/packets.h"

int welcome(client_t *client) {
    const char *msg = "Welcome! Please, identify.";
    s2c_confirm_payload_t payload = {
        .ok = 1,
        .msg_len = strlen(msg),
        .msg = msg
    };
    char buffer[512];
    int result = s2c_confirm(buffer, 512, client->session_id, &payload);
    if (result == 0) {
        printf("error serializing s2c_confirm_payload_t\n");
        return 1;
    }
    result = enqueue(client->socket, buffer, result);
    if (result != 0) {
        printf("error enqueueing s2c_confirm_payload_t\n");
        return 1;
    }
    return 0;
}

int client_status_update(client_t *client) {
    return welcome(client);
}