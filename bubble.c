#include "bubble.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

// Balon rengine karşılık gelen Raylib Color
Color GetColor(BubbleColor color) {
    switch (color) {
    case BUBBLE_RED: return (Color) { 255, 0, 0, 255 };      // Kırmızı
    case BUBBLE_GREEN: return (Color) { 0, 255, 0, 255 };    // Yeşil
    case BUBBLE_BLUE: return (Color) { 0, 0, 255, 255 };     // Mavi
    case BUBBLE_YELLOW: return (Color) { 255, 255, 0, 255 }; // Sarı
    case BUBBLE_PURPLE: return (Color) { 128, 0, 128, 255 }; // Mor
    case BUBBLE_NONE: return (Color) { 255, 255, 255, 255 }; // Beyaz
    default: return (Color) { 255, 255, 255, 255 };          // Varsayılan beyaz
    }
}

// Grid'i başlat
void InitBubbleGrid(BubbleGrid* grid) {
    float x_offset = BUBBLE_RADIUS;
    float y_offset = BUBBLE_RADIUS;
    float hex_width = BUBBLE_RADIUS * 2;  // Altıgenin genişliği
    float hex_height = BUBBLE_RADIUS * 1.73f;  // Altıgenin yüksekliği

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            grid->bubbles[r][c].active = (r < 5) ? 1 : 0;
            grid->bubbles[r][c].color = (r < 5) ? (BubbleColor)(rand() % BUBBLE_COLORS) : BUBBLE_NONE;
            float x = x_offset + c * hex_width + (r % 2) * BUBBLE_RADIUS;
            float y = y_offset + r * hex_height;
            grid->bubbles[r][c].pos = (Vector2){ x, y };
        }
    }
}

// Grid'i çiz
void DrawBubbleGrid(const BubbleGrid* grid) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                DrawCircleV(grid->bubbles[r][c].pos, BUBBLE_RADIUS, GetColor(grid->bubbles[r][c].color));
                DrawCircleLines(grid->bubbles[r][c].pos.x, grid->bubbles[r][c].pos.y, BUBBLE_RADIUS, DARKGRAY);
            }
        }
    }
}

// Balonu çiz
void DrawBubble(Bubble* bubble) {
    if (bubble->active) {
        DrawCircleV(bubble->pos, BUBBLE_RADIUS, GetColor(bubble->color));
        DrawCircleLines(bubble->pos.x, bubble->pos.y, BUBBLE_RADIUS, DARKGRAY);
    }
}

// Nişancıyı çiz
void DrawShooter(float angle, BubbleColor color) {
    Vector2 start = { 400, 550 };
    float rad = angle * (PI / 180.0f);
    Color bubbleColor = GetColor(color);

    // Noktalı çizgi için noktaları çiz
    for (int i = 0; i < 20; i++) {
        float distance = i * (BUBBLE_RADIUS * 1.5f);  // Noktalar arası mesafe balon boyutuna göre ayarlandı
        Vector2 dot = {
            start.x + cosf(rad) * distance,
            start.y - sinf(rad) * distance
        };

        // Ekran sınırlarını kontrol et
        if (dot.x < 0 || dot.x > 800 || dot.y < 0 || dot.y > 600) {
            // Köşeye çarpma durumunu göster
            if (dot.x < 0) {
                // Sol duvara çarpma
                float newAngle = PI - rad;
                float remainingDistance = distance - (dot.x / cosf(rad));
                Vector2 newDot = {
                    -dot.x,
                    dot.y - sinf(rad) * remainingDistance
                };
                DrawCircleV(newDot, BUBBLE_RADIUS / 5, bubbleColor);
            }
            else if (dot.x > 800) {
                // Sağ duvara çarpma
                float newAngle = PI - rad;
                float remainingDistance = distance - ((dot.x - 800) / cosf(rad));
                Vector2 newDot = {
                    1600 - dot.x,
                    dot.y - sinf(rad) * remainingDistance
                };
                DrawCircleV(newDot, BUBBLE_RADIUS / 5, bubbleColor);
            }
            break;
        }

        DrawCircleV(dot, BUBBLE_RADIUS / 5, bubbleColor);
    }

    // Ana ok çizgisi
    Vector2 end = {
        start.x + cosf(rad) * (BUBBLE_RADIUS * 2.5f),
        start.y - sinf(rad) * (BUBBLE_RADIUS * 2.5f)
    };
    DrawLineEx(start, end, BUBBLE_RADIUS / 5, bubbleColor);
}

// Grid'i temizle
void UnloadBubbleGrid(BubbleGrid* grid) {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            grid->bubbles[row][col].active = 0;
        }
    }
}

int IsGridFull(const BubbleGrid* grid) {
    for (int c = 0; c < GRID_COLS; c++) {
        if (grid->bubbles[GRID_ROWS - 1][c].active) return 1;
    }
    return 0;
}