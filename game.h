#ifndef GAME_H
#define GAME_H

#include "bubble.h"
#include "player.h"

// Oyun durumları
typedef enum { MENU, PLAYING, PAUSED, GAME_OVER } GameState;

// Oyun ana yapısı
typedef struct {
    BubbleGrid grid;
    Player player;
    int score;
    int level;
    int isGameOver;
} Game;

void InitGame(Game *game);
void UpdateGame(Game *game, GameState *state);
void DrawGame(const Game *game);
void ResetGame(Game *game);

#endif // GAME_H 