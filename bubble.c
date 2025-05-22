#include "bubble.h"
#include "constants.h"

// Balon rengine karşılık gelen Raylib Color
Color GetColor(BubbleColor color) {
    switch (color) {
    case BUBBLE_RED: return RED;
    case BUBBLE_GREEN: return GREEN;
    case BUBBLE_BLUE: return BLUE;
    case BUBBLE_YELLOW: return YELLOW;
    case BUBBLE_PURPLE: return PURPLE;
    default: return WHITE;
    }
}

// Grid'i başlat
void InitBubbleGrid(BubbleGrid* grid) {
    // Önce tüm grid'i temizle
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            grid->bubbles[row][col].active = 0;
        }
    }

    // Üst 3 satırı balonlarla doldur
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            // Tek satırlarda bir hücre kaydır
            if (row % 2 == 1 && col == GRID_COLS - 1) continue;
            
            grid->bubbles[row][col].active = 1;
            grid->bubbles[row][col].color = (BubbleColor)(rand() % BUBBLE_COLORS);
            grid->bubbles[row][col].pos.x = col * BUBBLE_RADIUS * 2 + BUBBLE_RADIUS;
            grid->bubbles[row][col].pos.y = row * BUBBLE_RADIUS * 2 + BUBBLE_RADIUS;
            if (row % 2 == 1) {
                grid->bubbles[row][col].pos.x += BUBBLE_RADIUS;
            }
        }
    }
}

// Grid'i çiz
void DrawBubbleGrid(BubbleGrid* grid) {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (grid->bubbles[row][col].active) {
                float x = col * BUBBLE_RADIUS * 2 + BUBBLE_RADIUS;
                float y = row * BUBBLE_RADIUS * 2 + BUBBLE_RADIUS;
                if (row % 2 == 1) x += BUBBLE_RADIUS;

                DrawCircle(x, y, BUBBLE_RADIUS, GetColor(grid->bubbles[row][col].color));
            }
        }
    }
}

// Balonu çiz
void DrawBubble(Bubble* bubble) {
    if (bubble->active) {
        DrawCircle(bubble->pos.x, bubble->pos.y, BUBBLE_RADIUS, GetColor(bubble->color));
    }
}

// Nişancıyı çiz
void DrawShooter(float angle) {
    Vector2 start = { SCREEN_WIDTH / 2, SCREEN_HEIGHT };
    Vector2 end = {
        start.x + cosf(angle) * 50,
        start.y + sinf(angle) * 50
    };
    DrawLineEx(start, end, 5, DARKGRAY);
}

// Çarpışma kontrolü
int CheckCollision(BubbleGrid* grid, Bubble* bubble) {
    // Üst sınır kontrolü
    if (bubble->pos.y < BUBBLE_RADIUS) {
        return 1;
    }

    // Grid ile çarpışma kontrolü
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (grid->bubbles[row][col].active) {
                float x = col * BUBBLE_RADIUS * 2 + BUBBLE_RADIUS;
                float y = row * BUBBLE_RADIUS * 2 + BUBBLE_RADIUS;
                if (row % 2 == 1) x += BUBBLE_RADIUS;

                float dx = bubble->pos.x - x;
                float dy = bubble->pos.y - y;
                float distance = sqrtf(dx * dx + dy * dy);

                if (distance < BUBBLE_RADIUS * 2) {
                    // Çarpışma noktasını bul
                    int newRow = (int)(bubble->pos.y / (BUBBLE_RADIUS * 2));
                    int newCol = (int)(bubble->pos.x / (BUBBLE_RADIUS * 2));
                    if (newRow % 2 == 1) newCol = (int)((bubble->pos.x - BUBBLE_RADIUS) / (BUBBLE_RADIUS * 2));

                    // Sınırları kontrol et
                    if (newRow >= 0 && newRow < GRID_ROWS && newCol >= 0 && newCol < GRID_COLS) {
                        grid->bubbles[newRow][newCol].active = 1;
                        grid->bubbles[newRow][newCol].color = bubble->color;
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Grid'i temizle
void UnloadBubbleGrid(BubbleGrid* grid) {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            grid->bubbles[row][col].active = 0;
        }
    }
}