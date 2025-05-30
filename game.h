#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "bubble.h"
#include "player.h"

// Forward declarations
typedef struct Game Game;

// Oyun durumları
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER,
    GAME_STATE_SETTINGS
} GameState;

// Oyun yapısı
struct Game {
    int score;
    int moves;
    bool gameOver;
    bool paused;
    BubbleGrid grid;
    Player player;
};

// Oyun fonksiyonları
void InitGame(Game* game);
void UpdateGame(Game* game);
void DrawGame(Game* game);
void UnloadGame(Game* game);

#endif // GAME_H