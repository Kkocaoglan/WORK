#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "ui.h"
#include <stdio.h>
#include <math.h>

#define MENU_BUBBLE_COUNT 30
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 60
#define SETTINGS_ICON_SIZE 40
#define SETTINGS_ICON_PADDING 20

typedef struct {
    Vector2 pos;
    float radius;
    float speed;
    Color color;
} MenuBubble;

static MenuBubble menuBubbles[MENU_BUBBLE_COUNT];
static bool initialized = false;
static Rectangle startButton;
static Rectangle settingsButton;

// Renk tanımlamaları
#define LIGHT_PINK (Color){ 255, 192, 203, 255 }
#define DARK_PINK (Color){ 219, 112, 147, 255 }
#define WHITE (Color){ 255, 255, 255, 255 }
#define BLACK (Color){ 0, 0, 0, 255 }

// Menü baloncuklarını başlat
static void InitMenuBubbles(void) {
    for (int i = 0; i < MENU_BUBBLE_COUNT; i++) {
        menuBubbles[i].pos = (Vector2){ GetRandomValue(0, GetScreenWidth()), GetRandomValue(GetScreenHeight(), GetScreenHeight() * 2) };
        menuBubbles[i].radius = GetRandomValue(10, 30);
        menuBubbles[i].speed = GetRandomValue(20, 60) / 100.0f;
        menuBubbles[i].color = (Color){ 255, 255, 255, GetRandomValue(50, 120) };
    }
}

// Menü baloncuklarını güncelle
static void UpdateMenuBubbles(float deltaTime) {
    for (int i = 0; i < MENU_BUBBLE_COUNT; i++) {
        menuBubbles[i].pos.y -= menuBubbles[i].speed * deltaTime * 60;

        if (menuBubbles[i].pos.y + menuBubbles[i].radius < 0) {
            menuBubbles[i].pos.x = GetRandomValue(0, GetScreenWidth());
            menuBubbles[i].pos.y = GetScreenHeight() + menuBubbles[i].radius;
            menuBubbles[i].radius = GetRandomValue(10, 30);
            menuBubbles[i].speed = GetRandomValue(20, 60) / 100.0f;
            menuBubbles[i].color.a = GetRandomValue(50, 120);
        }
    }
}

// Menü baloncuklarını çiz
static void DrawMenuBubbles(void) {
    for (int i = 0; i < MENU_BUBBLE_COUNT; i++) {
        DrawCircleV(menuBubbles[i].pos, menuBubbles[i].radius, menuBubbles[i].color);
        Color fadedWhite = (Color){ 255, 255, 255, 128 };
        DrawCircleLinesV(menuBubbles[i].pos, menuBubbles[i].radius, fadedWhite);
    }
}

// Ayarlar simgesini çiz
static void DrawSettingsIcon(Rectangle rect) {
    float centerX = rect.x + rect.width / 2;
    float centerY = rect.y + rect.height / 2;
    float radius = rect.width / 2 - 4;

    // Dış çember
    DrawCircleLinesV((Vector2) { centerX, centerY }, radius, WHITE);

    // İç çember
    DrawCircleV((Vector2) { centerX, centerY }, radius - 8, WHITE);

    // 3 adet dişli çark
    for (int i = 0; i < 3; i++) {
        float angle = i * (2 * PI / 3);
        float x = centerX + cosf(angle) * (radius - 4);
        float y = centerY + sinf(angle) * (radius - 4);

        // Dişli çarkın dış çemberi
        DrawCircleLinesV((Vector2) { x, y }, 6, WHITE);

        // Dişli çarkın iç çemberi
        DrawCircleV((Vector2) { x, y }, 4, WHITE);

        // Dişli çarkın dişleri
        for (int j = 0; j < 4; j++) {
            float toothAngle = angle + j * (PI / 2);
            float x1 = x + cosf(toothAngle) * 4;
            float y1 = y + sinf(toothAngle) * 4;
            float x2 = x + cosf(toothAngle) * 8;
            float y2 = y + sinf(toothAngle) * 8;
            DrawLineEx((Vector2) { x1, y1 }, (Vector2) { x2, y2 }, 2, WHITE);
        }
    }
}

// Ayarlar menüsünü çiz
void DrawSettingsUI(void) {
    // Yarı saydam arka plan
    Color semiTransparentPink = (Color){ 255, 192, 203, 200 };
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), semiTransparentPink);

    // Başlık
    const char* title = "SETTINGS";
    int titleSize = 40;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (GetScreenWidth() - titleWidth) / 2, 100, titleSize, DARK_PINK);

    // Geri dönme mesajı
    const char* backMsg = "Press ESC to go back";
    int msgSize = 20;
    int msgWidth = MeasureText(backMsg, msgSize);
    DrawText(backMsg, (GetScreenWidth() - msgWidth) / 2, 400, msgSize, DARK_PINK);
}

// Menü ekranı çizimi
void DrawMenuUI(void) {
    if (!initialized) {
        InitMenuBubbles();
        initialized = true;
    }

    float deltaTime = GetFrameTime();
    UpdateMenuBubbles(deltaTime);

    // Arka plan
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), LIGHT_PINK);

    // Animasyonlu baloncuklar
    DrawMenuBubbles();

    // Başlık
    const char* title = "Bubble Shooter";
    int titleSize = 50;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (GetScreenWidth() - titleWidth) / 2, 200, titleSize, DARK_PINK);

    // Start butonu
    startButton = (Rectangle){
        (GetScreenWidth() - BUTTON_WIDTH) / 2,
        300,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    // Buton arka planı
    DrawRectangleRec(startButton, DARK_PINK);

    // Buton kenarlığı
    DrawRectangleLinesEx(startButton, 2, WHITE);

    // Buton yazısı
    const char* buttonText = "START";
    int textSize = 30;
    int textWidth = MeasureText(buttonText, textSize);
    DrawText(buttonText,
        startButton.x + (BUTTON_WIDTH - textWidth) / 2,
        startButton.y + (BUTTON_HEIGHT - textSize) / 2,
        textSize,
        WHITE);

    // Ayarlar butonu
    settingsButton = (Rectangle){
        SETTINGS_ICON_PADDING,
        SETTINGS_ICON_PADDING,
        SETTINGS_ICON_SIZE,
        SETTINGS_ICON_SIZE
    };

    // Ayarlar simgesi
    DrawSettingsIcon(settingsButton);
}

// Start butonuna tıklandı mı kontrolü
bool IsStartButtonClicked(void) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), startButton);
}

// Ayarlar butonuna tıklandı mı kontrolü
bool IsSettingsButtonClicked(void) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), settingsButton);
}

// Duraklatma ekranı çizimi
void DrawPauseUI(void) {
    // Yarı saydam arka plan
    Color semiTransparentPink = (Color){ 255, 192, 203, 200 };
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), semiTransparentPink);

    // Pause yazısı
    const char* pauseText = "PAUSED";
    int textSize = 50;
    int textWidth = MeasureText(pauseText, textSize);
    DrawText(pauseText, (GetScreenWidth() - textWidth) / 2, 250, textSize, DARK_PINK);

    // Devam etme mesajı
    const char* continueMsg = "Press P to Continue";
    int msgSize = 30;
    int msgWidth = MeasureText(continueMsg, msgSize);
    DrawText(continueMsg, (GetScreenWidth() - msgWidth) / 2, 320, msgSize, DARK_PINK);
}

// Oyun sonu ekranı çizimi
void DrawGameOverUI(int score) {
    // Yarı saydam arka plan
    Color semiTransparentPink = (Color){ 255, 192, 203, 200 };
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), semiTransparentPink);

    // Game Over yazısı
    const char* gameOverText = "GAME OVER";
    int textSize = 50;
    int textWidth = MeasureText(gameOverText, textSize);
    DrawText(gameOverText, (GetScreenWidth() - textWidth) / 2, 200, textSize, DARK_PINK);

    // Skor
    char scoreText[32];
    sprintf(scoreText, "Score: %d", score);
    int scoreSize = 40;
    int scoreWidth = MeasureText(scoreText, scoreSize);
    DrawText(scoreText, (GetScreenWidth() - scoreWidth) / 2, 280, scoreSize, DARK_PINK);

    // Yeniden başlatma mesajı
    const char* restartMsg = "Press SPACE to Restart";
    int msgSize = 30;
    int msgWidth = MeasureText(restartMsg, msgSize);
    DrawText(restartMsg, (GetScreenWidth() - msgWidth) / 2, 350, msgSize, DARK_PINK);
}