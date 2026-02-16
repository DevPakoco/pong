#ifndef GAME_H
#define GAME_H

// ========== Region: Network ==========

#define PKTID_READY 0x01
#define PKTID_COUNTDOWN 0x02
#define PKTID_START 0x03

// ========== Region: Stuff ==========

#define GAME_COLS 117
#define GAME_ROWS 27

typedef struct Vec2 Vec2;

struct Vec2 {
    short X;
    short Y;
};



#endif