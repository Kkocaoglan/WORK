#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "bubble.h"

// Oyuncu yapısı
typedef struct {
    Vector2 pos;         // Oyuncunun konumu
    float angle;         // Nişangah açısı
    int shooting;        // Fırlatma durumu
    Bubble bubble;       // Mevcut balon
    Vector2 velocity;    // Balonun hızı
    int moves;           // Hamle sayısı
} Player;

// Oyuncu fonksiyonları
void InitPlayer(Player* player);
void NextBubble(Player* player, const BubbleGrid* grid);
void UpdatePlayer(Player* player, const BubbleGrid* grid);
void DrawPlayer(const Player* player, const BubbleGrid* grid);

#endif //PLAYER_H