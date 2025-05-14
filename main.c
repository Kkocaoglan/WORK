#include "raylib.h"
#include "game.h"
#include "ui.h"

int main(void) {
    // Raylib başlat
    InitWindow(800, 600, "Bubble Shooter");
    SetTargetFPS(60);

    GameState gameState = MENU;
    Game game;
    InitGame(&game);

    while (!WindowShouldClose()) {
        // Oyun durumu yönetimi
        switch (gameState) {
            case MENU:
                if (UpdateMenuUI()) gameState = PLAYING;
                break;
            case PLAYING:
                UpdateGame(&game, &gameState);
                break;
            case PAUSED:
                if (IsKeyPressed(KEY_P)) gameState = PLAYING;
                break;
            case GAME_OVER:
                if (UpdateGameOverUI()) {
                    ResetGame(&game);
                    gameState = MENU;
                }
                break;
        }

        // Çizim
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (gameState) {
            case MENU: DrawMenuUI(); break;
            case PLAYING: DrawGame(&game); break;
            case PAUSED: DrawPauseUI(); break;
            case GAME_OVER: DrawGameOverUI(game.score); break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
} 