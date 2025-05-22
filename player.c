#include "player.h"
#include <math.h>
#include <stdlib.h>
#include "raylib.h"
#include "bubble.h"

#define PLAYER_Y 550
#define MIN_ANGLE 30.0f
#define MAX_ANGLE 150.0f
#define BUBBLE_SPEED 8.0f

// Oyuncu başlatılır
void InitPlayer(Player* player) {
    player->pos = (Vector2){ 400, PLAYER_Y };
    player->angle = 90.0f;
    player->shooting = 0;
    player->bubble.active = 1;
    player->bubble.color = (BubbleColor)(rand() % BUBBLE_COLORS);
    player->bubble.pos = player->pos;
}

// Yeni balon hazırla
void NextBubble(Player* player) {
    player->shooting = 0;
    player->bubble.active = 1;
    player->bubble.color = (BubbleColor)(rand() % BUBBLE_COLORS);
    player->bubble.pos = player->pos;
    player->velocity = (Vector2){ 0, 0 };
}

// Oyuncu kontrolü ve balon fırlatma
void UpdatePlayer(Player* player, const BubbleGrid* grid) {
    if (!player->shooting) {
        // Açı ayarı
        if (IsKeyDown(KEY_LEFT)) player->angle -= 2.0f;
        if (IsKeyDown(KEY_RIGHT)) player->angle += 2.0f;
        if (player->angle < MIN_ANGLE) player->angle = MIN_ANGLE;
        if (player->angle > MAX_ANGLE) player->angle = MAX_ANGLE;
        // Fırlatma
        if (IsKeyPressed(KEY_SPACE)) {
            player->shooting = 1;
            float rad = player->angle * (PI / 180.0f);
            player->velocity.x = cosf(rad) * BUBBLE_SPEED;
            player->velocity.y = -sinf(rad) * BUBBLE_SPEED;
            player->bubble.pos = player->pos;
        }
    }
    else {
        // Fırlatılan balonun hareketi
        player->bubble.pos.x += player->velocity.x;
        player->bubble.pos.y += player->velocity.y;
        // Kenarlardan sekme
        if (player->bubble.pos.x < BUBBLE_RADIUS) {
            player->bubble.pos.x = BUBBLE_RADIUS;
            player->velocity.x *= -1;
        }
        if (player->bubble.pos.x > 800 - BUBBLE_RADIUS) {
            player->bubble.pos.x = 800 - BUBBLE_RADIUS;
            player->velocity.x *= -1;
        }
    }
}

// Oyuncu ve nişangahı çizer
void DrawPlayer(const Player* player) {
    // Fırlatma çizgisi
    float rad = player->angle * (PI / 180.0f);
    Vector2 end = {
        player->pos.x + cosf(rad) * 60,
        player->pos.y - sinf(rad) * 60
    };
    DrawLineV(player->pos, end, GRAY);
    // Fırlatılacak balon
    if (!player->shooting) {
        DrawCircleV(player->pos, BUBBLE_RADIUS, LIGHTGRAY);
        DrawCircleV(player->pos, BUBBLE_RADIUS - 2, GetColor(player->bubble.color));
    }
    else {
        DrawCircleV(player->bubble.pos, BUBBLE_RADIUS, GetColor(player->bubble.color));
    }
}
