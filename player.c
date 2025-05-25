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
void NextBubble(Player* player, const BubbleGrid* grid) {
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

// Nişan çizgisinin çarpışma kontrolü
static Vector2 CheckAimLineCollision(Vector2 start, Vector2 dir, const BubbleGrid* grid, int* bounceCount, Vector2* bouncePoints) {
    Vector2 current = start;
    float step = 5.0f; // Küçük adımlarla ilerle
    *bounceCount = 0;
    
    while (current.y > 0 && *bounceCount < 2) { // En fazla 2 sekme
        // Kenarlardan sekme kontrolü
        if (current.x < 0) {
            current.x = 0;
            bouncePoints[*bounceCount] = current;
            dir.x = -dir.x; // Yönü yansıt
            (*bounceCount)++;
            continue;
        }
        if (current.x > 800) {
            current.x = 800;
            bouncePoints[*bounceCount] = current;
            dir.x = -dir.x; // Yönü yansıt
            (*bounceCount)++;
            continue;
        }
        
        // Balonlarla çarpışma kontrolü
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (grid->bubbles[r][c].active) {
                    float dx = current.x - grid->bubbles[r][c].pos.x;
                    float dy = current.y - grid->bubbles[r][c].pos.y;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist < BUBBLE_RADIUS * 2) {
                        return current;
                    }
                }
            }
        }
        
        // Bir sonraki noktaya ilerle
        current.x += dir.x * step;
        current.y += dir.y * step;
    }
    
    return current; // Tavana çarptıysa veya maksimum sekme sayısına ulaştıysa
}

// Noktalı çizgi çizme yardımcı fonksiyonu
static void DrawDottedLine(Vector2 start, Vector2 end, Color color) {
    float totalDist = sqrtf(powf(end.x - start.x, 2) + powf(end.y - start.y, 2));
    int dotCount = (int)(totalDist / 20.0f); // Her 20 pikselde bir nokta
    
    for (int i = 1; i <= dotCount; i++) {
        float t = (float)i / dotCount;
        Vector2 dotPos = {
            start.x + (end.x - start.x) * t,
            start.y + (end.y - start.y) * t
        };
        // Noktaların boyutunu mesafeye göre ayarla
        float dotSize = 5.0f * (1.0f - t * 0.5f); // Uzaktaki noktalar daha küçük
        DrawCircleV(dotPos, dotSize, color);
    }
}

// Oyuncu ve nişangahı çizer
void DrawPlayer(const Player* player, const BubbleGrid* grid) {
    // Nişan çizgisi için yön vektörü
    float rad = player->angle * (PI / 180.0f);
    Vector2 start = player->pos;
    Vector2 dir = { cosf(rad), -sinf(rad) };
    
    // Çarpışma noktasını bul
    int bounceCount;
    Vector2 bouncePoints[2]; // En fazla 2 sekme noktası
    Vector2 end = CheckAimLineCollision(start, dir, grid, &bounceCount, bouncePoints);
    
    // Her bölüm için noktalı çizgi çiz
    Vector2 currentStart = start;
    for (int i = 0; i <= bounceCount; i++) {
        Vector2 currentEnd = (i < bounceCount) ? bouncePoints[i] : end;
        DrawDottedLine(currentStart, currentEnd, GetColor(player->bubble.color));
        currentStart = currentEnd;
    }

    // Fırlatılacak balon
    if (!player->shooting) {
        DrawCircleV(player->pos, BUBBLE_RADIUS, LIGHTGRAY);
        DrawCircleV(player->pos, BUBBLE_RADIUS - 2, GetColor(player->bubble.color));
    } else {
        DrawCircleV(player->bubble.pos, BUBBLE_RADIUS, GetColor(player->bubble.color));
    }
}