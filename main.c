#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "game.h"
#include "ui.h"

// Ekran boyutları
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(void) {
    // Pencereyi başlat
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Balon Patlat");
    SetTargetFPS(60);

    // Oyun durumunu başlat
    GameState gameState = GAME_STATE_MENU;
    Game game;
    InitGame(&game);

    // Ana döngü
    while (!WindowShouldClose()) {
        // Oyun durumu yönetimi
        switch (gameState) {
        case GAME_STATE_MENU:
            if (IsStartButtonClicked()) {
                gameState = GAME_STATE_PLAYING;
            }
            if (IsSettingsButtonClicked()) {
                gameState = GAME_STATE_SETTINGS;
            }
            break;
        case GAME_STATE_SETTINGS:
            if (IsKeyPressed(KEY_ESCAPE)) {
                gameState = GAME_STATE_MENU;
            }
            break;
        case GAME_STATE_PLAYING:
            UpdateGame(&game, &gameState);
            if (IsKeyPressed(KEY_P)) {
                gameState = GAME_STATE_PAUSED;
            }
            break;
        case GAME_STATE_PAUSED:
            if (IsKeyPressed(KEY_P)) {
                gameState = GAME_STATE_PLAYING;
            }
            break;
        case GAME_STATE_GAME_OVER:
            if (IsKeyPressed(KEY_SPACE)) {
                InitGame(&game);
                gameState = GAME_STATE_MENU;
            }
            break;
        }

        // Çizim
        BeginDrawing();
        ClearBackground((Color) { 255, 192, 203, 255 });  // Açık pembe renk

        switch (gameState) {
        case GAME_STATE_MENU:
            DrawMenuUI();
            break;
        case GAME_STATE_SETTINGS:
            DrawMenuUI();  // Arka plan için
            DrawSettingsUI();
            break;
        case GAME_STATE_PLAYING:
            DrawGame(&game);
            break;
        case GAME_STATE_PAUSED:
            DrawGame(&game);
            DrawPauseUI();
            break;
        case GAME_STATE_GAME_OVER:
            DrawGame(&game);
            DrawGameOverUI(game.score);
            break;
        }

        EndDrawing();
    }

    // Oyunu temizle
    UnloadGame(&game);
    CloseWindow();

    return 0;
}