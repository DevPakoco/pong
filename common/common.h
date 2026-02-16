#ifndef COMMON_H
#define COMMON_H

// ========== Region: Utils ==========

#define cls()       printf("\e[1;1H\e[2J");

#define VEC_ADD(v1, v2) ((typeof(v1)){v1.X + v2.X, v1.Y + v2.Y})
#define VEC_MUL(v1, v2) ((typeof(v1)){v1.X * v2.X, v1.Y * v2.Y})

// ========== Region: Linked List ==========

typedef struct LinkedListNode Node;
typedef struct LinkedListInfo Linfo;

struct LinkedListNode {
    void *pdata;
    Node *next;
    Node *prev;
};

struct LinkedListInfo {
    int size;
    Node *head;
    Node *tail;
};

Node* nodalloc();
Node* tail(Linfo *info);
Node* head(Linfo *info);
void pop(Node *node, Linfo *info, int deep);
void clear(Linfo *info, int deep);

#endif