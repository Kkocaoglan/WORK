#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "bubble.h"

// Oyun durumları
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER,
    GAME_STATE_SETTINGS
} GameState;

// Menü arayüzü
void DrawMenuUI(void);
void DrawSettingsUI(void);
bool IsStartButtonClicked(void);
bool IsSettingsButtonClicked(void);

// Oyun duraklatma arayüzü
void DrawPauseUI(void);

// Oyun sonu arayüzü
void DrawGameOverUI(int score);

#endif //UI_H