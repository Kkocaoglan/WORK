#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "game.h"

// Forward declarations
typedef struct Game Game;

// Menü fonksiyonları
void DrawMenuUI(void);
void DrawSettingsUI(void);
void DrawPauseUI(void);
void DrawGameOverUI(int score);
void DrawUI(const Game* game);

// Buton kontrol fonksiyonları
bool IsStartButtonClicked(void);
bool IsSettingsButtonClicked(void);

#endif // UI_H