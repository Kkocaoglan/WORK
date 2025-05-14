#include "bubble.h"
#include <stdlib.h>

// Balon rengine göre Raylib Color döndürür
Color GetColor(BubbleColor color) {
    switch (color) {
        case BUBBLE_RED: return RED;
        case BUBBLE_GREEN: return GREEN;
        case BUBBLE_BLUE: return BLUE;
        case BUBBLE_YELLOW: return YELLOW;
        case BUBBLE_PURPLE: return PURPLE;
        default: return BLANK;
    }
}

// Grid başlatılır: İlk 5 satır rastgele balonlarla doldurulur
void InitBubbleGrid(BubbleGrid *grid) {
    float x_offset = BUBBLE_RADIUS + 2; // Sol kenardan boşluk
    float y_offset = BUBBLE_RADIUS + 2; // Üst kenardan boşluk
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            grid->bubbles[r][c].active = (r < 5) ? 1 : 0;
            grid->bubbles[r][c].color = (r < 5) ? (BubbleColor)(rand() % BUBBLE_COLORS) : BUBBLE_NONE;
            float x = x_offset + c * BUBBLE_RADIUS * 2 + (r % 2) * BUBBLE_RADIUS;
            float y = y_offset + r * (BUBBLE_RADIUS * 1.73f);
            grid->bubbles[r][c].pos = (Vector2){ x, y };
        }
    }
}

// Griddeki tüm balonları çizer
void DrawBubbleGrid(const BubbleGrid *grid) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                DrawCircleV(grid->bubbles[r][c].pos, BUBBLE_RADIUS, GetColor(grid->bubbles[r][c].color));
                DrawCircleLines(grid->bubbles[r][c].pos.x, grid->bubbles[r][c].pos.y, BUBBLE_RADIUS, DARKGRAY);
            }
        }
    }
}

// Gridin en alt satırı doluysa oyun biter
int IsGridFull(const BubbleGrid *grid) {
    for (int c = 0; c < GRID_COLS; c++) {
        if (grid->bubbles[GRID_ROWS-1][c].active) return 1;
    }
    return 0;
} 