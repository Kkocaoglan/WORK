#ifndef COLLISION_H
#define COLLISION_H

#include "bubble.h"

// Fırlatılan balonun griddeki balonlara çarpıp çarpmadığını kontrol eder
int CheckBubbleCollision(const Bubble *bubble, const BubbleGrid *grid);

// Balonu gridde uygun yere yerleştirir
void PlaceBubble(Bubble *bubble, BubbleGrid *grid);

// Bağlantılı balonları DFS/BFS ile bulup patlatır, patlayan balon sayısını döndürür
int PopConnectedBubbles(BubbleGrid *grid);

// Tavana bağlı olmayan balonları düşürür, düşen balon sayısı kadar puan eklenebilir
void DropFloatingBubbles(BubbleGrid *grid, int *score);

#endif // COLLISION_H 