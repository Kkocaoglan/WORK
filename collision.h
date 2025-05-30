#ifndef COLLISION_H
#define COLLISION_H

#include "bubble.h"

// Fırlatılan balonun griddeki balonlara çarpıp çarpmadığını kontrol eder
int CheckBubbleCollision(const BubbleGrid* grid, const Bubble* bubble);

// Balonu gridde uygun yere yerleştirir
void PlaceBubble(Bubble* bubble, BubbleGrid* grid, int* outRow, int* outCol);

// Aynı renkteki bağlı balonları patlatır
int PopConnectedBubbles(BubbleGrid* grid, int row, int col);

// Havada kalan balonları düşürür
int DropFloatingBubbles(BubbleGrid* grid);

// Patlama durumunu sıfırlar
void ResetExplosionState(void);

// Bağlı topları bul
void FindConnectedBubbles(BubbleGrid* grid, int row, int col, BubbleColor color, bool visited[GRID_ROWS][GRID_COLS], int* matchCount, int matches[][2]);

#endif // COLLISION_H