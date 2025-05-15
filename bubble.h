#ifndef BUBBLE_H
#define BUBBLE_H

#include "raylib.h"

// Grid boyutları ve balon yarıçapı
#define GRID_ROWS 8
#define GRID_COLS 19
#define BUBBLE_RADIUS 20
#define BUBBLE_COLORS 5

// Balon renkleri
typedef enum {
    BUBBLE_RED,
    BUBBLE_GREEN,
    BUBBLE_BLUE,
    BUBBLE_YELLOW,
    BUBBLE_PURPLE,
    NUM_COLORS
} BubbleColor;

// Tek bir balonun yapısı
typedef struct {
    Vector2 pos;         // Ekrandaki konumu
    Vector2 velocity;
    BubbleColor color;   // Rengi
    int active;          // Aktif mi?
} Bubble;

// Altıgen grid yapısı
typedef struct {
    Bubble bubbles[GRID_ROWS][GRID_COLS];
} BubbleGrid;

// Fonksiyon prototipleri
void InitBubbleGrid(BubbleGrid* grid);
void DrawBubbleGrid(BubbleGrid* grid);
void DrawBubble(Bubble* bubble);
void DrawShooter(float angle);
int CheckCollision(BubbleGrid* grid, Bubble* bubble);
void UnloadBubbleGrid(BubbleGrid* grid);
int IsGridFull(const BubbleGrid* grid);
Color GetColor(BubbleColor color); // Balon rengine karşılık gelen Raylib Color

#endif // BUBBLE_H 