#include "raylib.h"
#include "game.h"
#include "ui.h"

int main(void) {
    // Pencere başlatma
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bubble Shooter");
    SetTargetFPS(60);

    // Oyun yapısını başlat
    Game game;
    InitGame(&game);
    GameState state = GAME_STATE_MENU;

    // Ana döngü
    while (!WindowShouldClose()) {
        // Oyun durumuna göre güncelleme
        switch (state) {
            case GAME_STATE_MENU:
                if (UpdateMenuUI()) {
                    state = GAME_STATE_PLAYING;
                }
                break;

            case GAME_STATE_PLAYING:
                UpdateGame(&game, &state);
                if (IsKeyPressed(KEY_P)) {
                    state = GAME_STATE_PAUSED;
                }
                break;

            case GAME_STATE_PAUSED:
                if (IsKeyPressed(KEY_P)) {
                    state = GAME_STATE_PLAYING;
                }
                break;

            case GAME_STATE_LEVEL_COMPLETE:
                // Level tamamlandı, sonraki levele geç
                StartNextLevel(&game);
                state = GAME_STATE_PLAYING;
                break;

            case GAME_STATE_GAME_OVER:
                if (UpdateGameOverUI()) {
                    ResetGame(&game);
                    state = GAME_STATE_PLAYING;
                }
                break;
        }

        // Çizim
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (state) {
            case GAME_STATE_MENU:
                DrawMenuUI();
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

    // Temizlik
    UnloadGame(&game);
    CloseWindow();

    return 0;
}