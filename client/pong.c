#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include "globals.h"
#include "../common/common.h"

typedef struct Board Board;
typedef struct Score Score;
typedef struct GameState GameState;

struct Score {
    int player1;
    int player2;
};

struct GameState {
    int over;
    COORD lPaddlePos;
    COORD rPaddlePos;
    COORD ballPos;
    COORD ballDirection;
};

struct Board {
    COORD position;
    COORD size;
    Score score;
    int paddleSize;
    int playWidth;
    int playHeight;
};

Board* BoardCreate(COORD position, COORD size) {
    Board *board = calloc(1, sizeof(*board));
    if (board == NULL) return NULL;

    board->position    = position;
    board->size        = size;
    board->paddleSize = 3;
    board->playWidth   = size.X - 2;
    board->playHeight  = size.Y - 2;

    return board;
}

GameState initialGameState(const Board *board) {
    int mid_y = (board->playHeight - board->paddleSize) / 2;
    return (GameState){
        .over          = 0,
        .lPaddlePos    = {1, mid_y},
        .rPaddlePos    = {board->playWidth - 2, mid_y},
        .ballPos       = {board->playWidth / 2, board->playHeight / 2},
        .ballDirection = {2, 1}
    };
}

COORD board_to_screen(const Board *board, COORD local) {
    return (COORD){
        board->position.X + local.X + 1,   // +1 for left border
        board->position.Y + local.Y + 1    // +1 for top border
    };
}

int is_paddle_hit(int paddleSize, const GameState *gs) {
    int ballNextX = gs->ballPos.X + gs->ballDirection.X;
    int ballY     = gs->ballPos.Y;
    if (ballNextX <= gs->lPaddlePos.X) {
        int top    = gs->lPaddlePos.Y;
        int bottom = gs->lPaddlePos.Y + paddleSize - 1;
        if (ballY >= top && ballY <= bottom) return 1;
    }
    if (ballNextX >= gs->rPaddlePos.X) {
        int top    = gs->rPaddlePos.Y;
        int bottom = gs->rPaddlePos.Y + paddleSize - 1;
        if (ballY >= top && ballY <= bottom) return 1;
    }
    return 0;
}

// ========== Region: Drawing ==========

void drawBorder(const Board *board) {
    COORD pos = board->position;
    SetConsoleCursorPosition(HOUT, pos);
    for (int i = 0; i < board->size.X; i++) printf("▄");

    for (int y = 1; y < board->size.Y - 1; y++) {
        pos.Y = board->position.Y + y;
        SetConsoleCursorPosition(HOUT, pos);
        printf("█");
        for (int x = 0; x < board->size.X - 2; x++) printf(" ");
        printf("█");
    }

    pos.Y = board->position.Y + board->size.Y - 1;
    SetConsoleCursorPosition(HOUT, pos);
    for (int i = 0; i < board->size.X; i++) printf("▀");
}

void erase_at(const Board *board, COORD local) {
    COORD screen = board_to_screen(board, local);
    SetConsoleCursorPosition(HOUT, screen);
    printf(" ");
}

void draw_paddle_piece(const Board *board, COORD local) {
    COORD screen = board_to_screen(board, local);
    SetConsoleCursorPosition(HOUT, screen);
    printf("█");
}

void update_paddle(const Board *board, COORD *old_pos, COORD *new_pos, int paddle_size) {
    if (old_pos->Y == new_pos->Y) return;
    int old_top    = old_pos->Y;
    int old_bottom = old_pos->Y + paddle_size - 1;
    int new_top    = new_pos->Y;
    int new_bottom = new_pos->Y + paddle_size - 1;
    int dy = new_pos->Y - old_pos->Y;
    if (dy < 0) {
        for (int y = old_bottom; y >= old_top; y--) {
            if (y < new_top || y > new_bottom)
                erase_at(board, (COORD){old_pos->X, y});
        }
    } else {
        for (int y = old_top; y <= old_bottom; y++) {
            if (y < new_top || y > new_bottom)
                erase_at(board, (COORD){old_pos->X, y});
        }
    }
    for (int y = new_top; y <= new_bottom; y++) {
        if (y < old_top || y > old_bottom) 
        draw_paddle_piece(board, (COORD){new_pos->X, y});
    }
}

void update_ball(const Board *board, COORD old_ball, COORD new_ball) {
    if (old_ball.X == new_ball.X && old_ball.Y == new_ball.Y) return;
    erase_at(board, old_ball);
    COORD screen = board_to_screen(board, new_ball);
    SetConsoleCursorPosition(HOUT, screen);
    printf("⬤");
}

void draw_full_paddle(const Board *board, COORD pos, int paddle_size) {
    for (int y = 0; y < paddle_size; y++)
        draw_paddle_piece(board, (COORD){pos.X, pos.Y + y});
}

void draw_full_ball(const Board *board, COORD pos) {
    COORD screen = board_to_screen(board, pos);
    SetConsoleCursorPosition(HOUT, screen);
    printf("⬤");
}

void drawGameElementsIncremental(const Board *board, const GameState *gs, const GameState *oldgs) {
    if (oldgs == NULL) {
        draw_full_ball(board, gs->ballPos);
        draw_full_paddle(board, gs->lPaddlePos, board->paddleSize);
        draw_full_paddle(board, gs->rPaddlePos, board->paddleSize);
        return;
    }
    update_ball(board, oldgs->ballPos, gs->ballPos);
    update_paddle(board, &oldgs->lPaddlePos, &gs->lPaddlePos, board->paddleSize);
    update_paddle(board, &oldgs->rPaddlePos, &gs->rPaddlePos, board->paddleSize);
}

void redraw(Board *board, GameState *gs, GameState *oldgs) {
    drawGameElementsIncremental(board, gs, oldgs);
}

// ========== Region: Game Flow ==========

void movePaddle(Board *board, COORD *paddle, int dy) {
    int new_y = paddle->Y + dy;
    if (new_y < 0)
        new_y = 0;
    if (new_y > board->playHeight - board->paddleSize)
        new_y = board->playHeight - board->paddleSize;
    paddle->Y = new_y;
}

void moveBall(GameState *gs) {
    gs->ballPos = VEC_ADD(gs->ballPos, gs->ballDirection);
}

void checkCollisions(Board *board, GameState *gs) {
    if (gs->ballPos.Y == 0 || gs->ballPos.Y == board->playHeight - 1)
        gs->ballDirection.Y *= -1;

    if (is_paddle_hit(board->paddleSize, gs))
        gs->ballDirection.X *= -1;

    if (gs->ballPos.X == 0 || gs->ballPos.X == board->playWidth - 1)
        gs->over = 1;
}

void Start(Board *board) {
    GameState gs = initialGameState(board);
    GameState oldgs;
    GameState *oldptr = NULL;
    drawBorder(board);
    while (!gs.over) {
        if (GetAsyncKeyState('W') & 0x8000)
            movePaddle(board, &gs.lPaddlePos, -1);
        if (GetAsyncKeyState('S') & 0x8000)
            movePaddle(board, &gs.lPaddlePos, +1);
        FlushConsoleInputBuffer(HIN);
        moveBall(&gs);
        movePaddle(board, &gs.rPaddlePos, gs.ballPos.Y - gs.rPaddlePos.Y - 1);
        checkCollisions(board, &gs);
        redraw(board, &gs, oldptr);
        oldgs = gs;
        oldptr = &oldgs;
        Sleep(50);
    }
}
