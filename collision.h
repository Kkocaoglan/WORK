#ifndef COLLISION_H  
#define COLLISION_H  

#include "bubble.h"  

// Fırlatılan balonun griddeki balonlara çarpıp çarpmadığını kontrol eder  
int CheckBubbleCollision(const Bubble* bubble, const BubbleGrid* grid);  

// Balonu gridde uygun yere yerleştirir, yerleştirilen satır ve sütunu döndürür  
void PlaceBubble(Bubble* bubble, BubbleGrid* grid, int* outRow, int* outCol);  

// Bağlantılı balonları verilen konumdan DFS/BFS ile bulup patlatır, patlayan balon sayısını döndürür  
int PopConnectedBubbles(BubbleGrid* grid, int row, int col);  

// Tavana bağlı olmayan balonları düşürür, düşen balon sayısını döndürür  
int DropFloatingBubbles(BubbleGrid* grid);  

#endif // COLLISION_H