#ifndef PLAYER_H
#define PLAYER_H

#include "bubble.h"

// Oyuncu yapısı: Fırlatma pozisyonu, açı, mevcut balon ve fırlatma durumu
typedef struct {
    Vector2 pos;      // Fırlatma noktası
    float angle;      // Fırlatma açısı (derece)
    Bubble bubble;    // Fırlatılacak balon
    int shooting;     // Balon fırlatılıyor mu?
} Player;

void InitPlayer(Player *player);
void UpdatePlayer(Player *player, const BubbleGrid *grid);
void DrawPlayer(const Player *player);
void NextBubble(Player *player);

#endif // PLAYER_H 