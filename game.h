#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "bubble.h"

// Oyun durumları
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER
} GameState;

// Oyun yapısı
typedef struct {
    int score;
    BubbleGrid grid;
    float shooterAngle;
    Bubble currentBubble;
} Game;

// Oyun fonksiyonları
void InitGame(Game* game);
void UpdateGame(Game* game, GameState* state);
void DrawGame(Game* game);
void UnloadGame(Game* game);

#endif // GAME_H 