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

    // Raylib ses sistemini başlat
    InitAudioDevice();

    // Arka plan müziğini yükle ve başlat
    Music backgroundMusic = LoadMusicStream("resources/sounds/rainyday.wav");

    // Müzik yüklendi mi kontrol et
    if (backgroundMusic.stream.buffer != NULL) {
        SetMusicVolume(backgroundMusic, 0.5f);  // Ses seviyesi %50
        PlayMusicStream(backgroundMusic);
    }

    // Oyun durumunu başlat
    GameState gameState = GAME_STATE_MENU;
    Game game;
    InitGame(&game);

    // Ana döngü
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Müziği güncelle (ÖNEMLİ - her frame'de çağırılmalı!)
        if (backgroundMusic.stream.buffer != NULL) {
            UpdateMusicStream(backgroundMusic);

            // Müzik bitti mi? Tekrar başlat (döngü için)
            if (!IsMusicStreamPlaying(backgroundMusic)) {
                PlayMusicStream(backgroundMusic);
            }
        }

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
            UpdateGame(&game);
            if (IsKeyPressed(KEY_P)) {
                gameState = GAME_STATE_PAUSED;
            }
            if (game.gameOver) {
                gameState = GAME_STATE_GAME_OVER;
            }
            break;

        case GAME_STATE_PAUSED:
            if (IsKeyPressed(KEY_P)) {
                gameState = GAME_STATE_PLAYING;
            }
            break;

        case GAME_STATE_GAME_OVER:
            if (IsRestartButtonClicked()) {
                InitGame(&game);
                gameState = GAME_STATE_PLAYING;
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

    // Temizlik
    if (backgroundMusic.stream.buffer != NULL) {
        UnloadMusicStream(backgroundMusic);
    }
    CloseAudioDevice();
    UnloadGame(&game);
    CloseWindow();
    return 0;
}