#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "bubble.h"
#include "player.h"
#include "ui.h"

// Oyun yapısı
typedef struct {
    int score;
    int moves;
    bool gameOver;
    bool paused;
    BubbleGrid grid;
    Player player;
} Game;

// Oyun fonksiyonları
void InitGame(Game* game);
void UpdateGame(Game* game, GameState* state);
void DrawGame(Game* game);
void UnloadGame(Game* game);

#endif //GAME_H