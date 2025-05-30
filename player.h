#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "bubble.h"
#include "collision.h"

#define BUBBLE_RADIUS 20
#define BUBBLE_COLORS 6
#define BOMB_BUTTON_SIZE 40
#define BOMB_BUTTON_MARGIN 20

// Oyuncu yapısı
typedef struct {
    Vector2 pos;         // Oyuncunun konumu
    float angle;         // Nişangah açısı
    int shooting;        // Fırlatma durumu
    Bubble bubble;       // Mevcut balon
    Vector2 velocity;    // Balonun hızı
    int moves;           // Hamle sayısı
    int bombCount;       // Bomba sayısı
    bool canUseBomb;     // Bomba kullanılabilir mi
    int score;           // Oyuncunun puanı
    bool bombActive;     // Bomba butonu aktif mi kontrolü
} Player;

// Oyuncu fonksiyonları
void InitPlayer(Player* player);
void NextBubble(Player* player, const BubbleGrid* grid);
void UpdatePlayer(Player* player, BubbleGrid* grid);
void DrawPlayer(const Player* player, const BubbleGrid* grid);
void UnloadPlayer(Player* player);
void UseBomb(Player* player);
void DrawBombButton(const Player* player);
void HandleBombExplosion(Player* player, BubbleGrid* grid, Vector2 hitPos);

#endif // PLAYER_H