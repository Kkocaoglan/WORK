#ifndef BUBBLE_H
#define BUBBLE_H

#include "raylib.h"

// Grid boyutları
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
    BUBBLE_NONE
} BubbleColor;

// Balon yapısı
typedef struct {
    Vector2 pos;
    BubbleColor color;
    bool active;
} Bubble;

// Balon ızgarası yapısı
typedef struct {
    Bubble bubbles[GRID_ROWS][GRID_COLS];
} BubbleGrid;

// Fonksiyon prototipleri
void InitBubbleGrid(BubbleGrid* grid);
void DrawBubbleGrid(const BubbleGrid* grid);
void DrawBubble(Bubble* bubble);
void DrawShooter(float angle, BubbleColor color);
void UnloadBubbleGrid(BubbleGrid* grid);
int IsGridFull(const BubbleGrid* grid);
Color GetColor(BubbleColor color);
void CreateExplosionParticle(BubbleGrid* grid, Vector2 pos, Color color);
void UpdateParticles(void);
void DrawParticles(void);
void ExplodeAllBubbles(BubbleGrid* grid);

#endif // BUBBLE_H