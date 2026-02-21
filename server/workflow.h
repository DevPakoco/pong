#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <windows.h>

typedef enum {
    WORKFLOW_LOGIN,
    WORKFLOW_QUEUE,
    WORKFLOW_GAME
} workflow_t;

typedef enum {
    LOGIN_UNLOGGED,
    LOGIN_REQUESTED,
    LOGIN_FAILED,
    LOGIN_SUCCESS
} login_state_t;

typedef enum {
    QUEUE_IDLE,
    QUEUE_SEARCHING,
    QUEUE_FOUND
} queue_state_t;

typedef enum {
    GAME_PLAYING
} game_state_t;

typedef struct {
    workflow_t workflow;
    union {
        login_state_t login_state;
        queue_state_t queue_state;
        game_state_t game_state;
    };
} workflow_state_t;

typedef struct {
    SOCKET socket;
    int session_id;
    char username[32];
    workflow_state_t workflow_state;
    char pending;
    char *received;
} client_t;

int client_status_update(client_t *client);

#endif