#include <stdio.h>
#include <stdlib.h>
#include "common.h"

// ========== Region: Linked List ==========

Node* nodalloc() {
    return calloc(1, sizeof(Node));
}

void single(Node *node, Linfo *info) {
    info->size = 1;
    info->head = node;
    info->tail = node;
}

Node* tail(Linfo *info) {
    Node *node = nodalloc();
    if (info->size == 0) {
        single(node, info);
    } else {
        Node *tail = info->tail;
        node->prev = tail;
        tail->next = node;
        info->tail = node;
        info->size++;
    }
    return node;
}

Node* head(Linfo *info) {
    Node *node = nodalloc();
    if (info->size == 0) {
        single(node, info);
    } else {
        Node *head = info->head;
        head->prev = node;
        node->next = head;
        info->head = node;
        info->size++;
    }
    return node;
}

void pop(Node *node, Linfo *info, int deep) {
    Node *next = node->next;
    Node *prev = node->prev;
    if (prev != NULL) prev->next = next;
    else info->head = next;
    if (next != NULL) next->prev = prev;
    else info->tail = prev;
    info->size--;
    if (deep) free(node->pdata);
    free(node);
}

void clear(Linfo *info, int deep) {
    Node *pn = info->head;
    while (pn != NULL) {
        if (deep) free(pn->pdata);
        Node *next = pn->next;
        free(pn);
        pn = next;
    }
    info->head = NULL;
    info->tail = NULL;
    info->size = 0;
}

