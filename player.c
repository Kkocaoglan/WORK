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

// Mouse pozisyonundan açı hesapla
float CalculateAngleFromMouse(Vector2 mousePos, Vector2 playerPos) {
    float dx = mousePos.x - playerPos.x;
    float dy = playerPos.y - mousePos.y; // Y koordinatı ters olduğu için çıkarma yapıyoruz
    float angle = atan2f(dy, dx) * (180.0f / PI);
    
    // Açıyı sınırla
    if (angle < MIN_ANGLE) angle = MIN_ANGLE;
    if (angle > MAX_ANGLE) angle = MAX_ANGLE;
    
    return angle;
}

// Oyuncu kontrolü ve balon fırlatma
void UpdatePlayer(Player* player, const BubbleGrid* grid) {
    if (!player->shooting) {
        // Mouse pozisyonundan açı hesapla
        Vector2 mousePos = GetMousePosition();
        float mouseAngle = CalculateAngleFromMouse(mousePos, player->pos);
        
        // Klavye ile açı değişimi
        float angleChange = 0.0f;
        if (IsKeyDown(KEY_LEFT)) angleChange -= 2.0f;
        if (IsKeyDown(KEY_RIGHT)) angleChange += 2.0f;
        
        // Mouse ve klavye açılarını birleştir
        if (angleChange != 0.0f) {
            // Klavye ile değişim varsa, mevcut açıyı değiştir
            player->angle += angleChange;
        } else {
            // Klavye ile değişim yoksa, mouse açısını kullan
            // Ancak ani değişimleri önlemek için yumuşak geçiş yap
            float angleDiff = mouseAngle - player->angle;
            if (fabsf(angleDiff) > 180.0f) {
                if (angleDiff > 0) angleDiff -= 360.0f;
                else angleDiff += 360.0f;
            }
            player->angle += angleDiff * 0.2f; // Yumuşak geçiş için 0.2 çarpanı
        }
        
        // Açı sınırlaması
        if (player->angle < MIN_ANGLE) player->angle = MIN_ANGLE;
        if (player->angle > MAX_ANGLE) player->angle = MAX_ANGLE;

        // Fırlatma (hem mouse hem klavye ile)
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)) {
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
    // Noktalı nişan çizgisi
    float rad = player->angle * (PI / 180.0f);
    Vector2 start = player->pos;
    Vector2 dir = { cosf(rad), -sinf(rad) };
    int dotCount = 10;
    float dotSpacing = 20.0f;

    for (int i = 1; i <= dotCount; i++) {
        Vector2 dotPos = {
            start.x + dir.x * dotSpacing * i,
            start.y + dir.y * dotSpacing * i
        };
        DrawCircleV(dotPos, 5, GetColor(player->bubble.color)); // Balonun rengiyle aynı
    }

    // Fırlatılacak balon
    if (!player->shooting) {
        DrawCircleV(player->pos, BUBBLE_RADIUS, LIGHTGRAY);
        DrawCircleV(player->pos, BUBBLE_RADIUS - 2, GetColor(player->bubble.color));
    } else {
        DrawCircleV(player->bubble.pos, BUBBLE_RADIUS, GetColor(player->bubble.color));
    }
}
