#ifndef PACKETS_H
#define PACKETS_H

// ========== Region: Server to Client Packets ==========
enum S2CPktId {
    S2C_ID = 1
};

struct {
    const S2CPktId pktId = S2C_ID;
    int id;
} S2CId;

// ========== Region: Client to Server Packets ==========

enum C2SPktId {
    C2S_ID = 1
};

struct {
    const C2SPktId pktId = C2S_ID;
    int id;
    char name[32];
} C2SId;

#endif