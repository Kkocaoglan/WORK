#define _CRT_SECURE_NO_WARNINGS
#include "ui.h"
#include "raylib.h"
#include <stdio.h>

// Menü ekranı çizimi
void DrawMenuUI(void) {
    DrawText("BUBBLE SHOOTER", 220, 180, 48, DARKBLUE);
    DrawText("Baslamak icin SPACE'e basin", 240, 300, 24, GRAY);
}

// Menü ekranı güncelleme (başlatma için)
int UpdateMenuUI(void) {
    return IsKeyPressed(KEY_SPACE);
}

// Skor gösterimi
void DrawScoreUI(int score) {
    char buf[64];
    sprintf(buf, "Skor: %d", score);
    DrawText(buf, 20, 20, 32, DARKGRAY);
}

// Level bilgisi gösterimi
void DrawLevelUI(int level, int targetScore) {
    char buf[64];
    sprintf(buf, "Level: %d", level);
    DrawText(buf, 20, 60, 32, DARKGRAY);

    sprintf(buf, "Hedef: %d", targetScore);
    DrawText(buf, 20, 100, 32, DARKGRAY);
}

// Duraklatma ekranı çizimi
void DrawPauseUI(void) {
    DrawText("DURAKLATILDI", 300, 250, 36, ORANGE);
    DrawText("Devam icin P'ye basin", 270, 320, 24, GRAY);
}

// Level tamamlama ekranı çizimi
void DrawLevelCompleteUI(int level, int score) {
    char buf[64];
    sprintf(buf, "Level %d Tamamlandi!", level);
    DrawText(buf, 220, 220, 36, GREEN);

    sprintf(buf, "Skorunuz: %d", score);
    DrawText(buf, 270, 280, 24, DARKGRAY);

    DrawText("Sonraki level icin SPACE'e basin", 180, 340, 24, GRAY);
}

// Oyun sonu ekranı çizimi
void DrawGameOverUI(int score) {
    char buf[64];
    sprintf(buf, "Oyun Bitti! Skorunuz: %d", score);
    DrawText(buf, 220, 220, 36, RED);
    DrawText("Yeniden baslamak icin SPACE'e basin", 180, 300, 24, GRAY);
}

// Oyun sonu ekranı güncelleme (yeniden başlatma için)
int UpdateGameOverUI(void) {
    return IsKeyPressed(KEY_SPACE);
}