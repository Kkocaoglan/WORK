#include "player.h"
#include <math.h>
#include <stdlib.h>
#include "raylib.h"
#include "bubble.h"

#define PLAYER_Y 550
#define MIN_ANGLE 0.0f
#define MAX_ANGLE 180.0f
#define BUBBLE_SPEED 15.0f

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

    // Ekranda kalan top sayısını hesapla
    int remainingBubblesOnScreen = 0;
    int colorCounts[BUBBLE_COLORS] = { 0 };
    int availableColors = 0;  // Kullanılabilir renk sayısı
    int availableColorIndices[BUBBLE_COLORS] = { 0 };  // Kullanılabilir renklerin indeksleri

    // Ekrandaki topları say
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                remainingBubblesOnScreen++;
                colorCounts[grid->bubbles[r][c].color]++;
            }
        }
    }

    // Eğer ekranda 15 veya daha az top varsa, akıllı renk seçimi yap
    if (remainingBubblesOnScreen <= 15) {
        // Önce kullanılabilir renkleri bul
        for (int i = 0; i < BUBBLE_COLORS; i++) {
            if (colorCounts[i] > 0) {
                availableColorIndices[availableColors] = i;
                availableColors++;
            }
        }

        // Eğer kullanılabilir renk varsa, onlardan birini seç
        if (availableColors > 0) {
            int randomIndex = rand() % availableColors;
            player->bubble.color = (BubbleColor)availableColorIndices[randomIndex];
        }
        else {
            // Hiç renk kalmadıysa varsayılan olarak kırmızı rengi seç
            player->bubble.color = BUBBLE_RED;
        }
    }
    else {
        // Normal rastgele renk seçimi
        player->bubble.color = (BubbleColor)(rand() % BUBBLE_COLORS);
    }

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
        }
        else {
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

// Tek bir ray'in çarpışma kontrolü (balonlar için)
static int CheckRayBubbleCollision(Vector2 start, Vector2 dir, const BubbleGrid* grid, Vector2* hitPoint) {
    float step = 2.0f; // Küçük adımlarla ilerle
    Vector2 current = start;

    // Maksimum mesafe (ekran boyutu + biraz marj)
    float maxDistance = 1000.0f;
    float traveled = 0.0f;

    while (traveled < maxDistance && current.y > 0) {
        // Balonlarla çarpışma kontrolü
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (grid->bubbles[r][c].active) {
                    float dx = current.x - grid->bubbles[r][c].pos.x;
                    float dy = current.y - grid->bubbles[r][c].pos.y;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist < BUBBLE_RADIUS * 2) {
                        *hitPoint = current;
                        return 1; // Çarpışma bulundu
                    }
                }
            }
        }

        // Bir sonraki noktaya ilerle
        current.x += dir.x * step;
        current.y += dir.y * step;
        traveled += step;
    }

    // Tavana çarptıysa veya maksimum mesafeye ulaştıysa
    *hitPoint = current;
    return 0; // Balon çarpışması yok
}

// Nişan çizgisinin tam yolu hesapla
static void CalculateAimPath(Vector2 start, Vector2 initialDir, const BubbleGrid* grid,
    Vector2* segments, int* segmentCount) {
    Vector2 currentStart = start;
    Vector2 currentDir = initialDir;
    *segmentCount = 0;
    int maxBounces = 3; // Maksimum sekme sayısı

    segments[(*segmentCount)++] = currentStart; // İlk nokta

    for (int bounce = 0; bounce <= maxBounces; bounce++) {
        Vector2 hitPoint;
        int hitBubble = CheckRayBubbleCollision(currentStart, currentDir, grid, &hitPoint);

        // Duvar çarpışması kontrolü
        float wallHitX = -1;
        float wallHitY = -1;

        // Sol duvar
        if (currentDir.x < 0) {
            float t = (BUBBLE_RADIUS - currentStart.x) / currentDir.x;
            if (t > 0) {
                float y = currentStart.y + currentDir.y * t;
                if (y > 0) {
                    wallHitX = BUBBLE_RADIUS;
                    wallHitY = y;
                }
            }
        }
        // Sağ duvar
        else if (currentDir.x > 0) {
            float t = (800 - BUBBLE_RADIUS - currentStart.x) / currentDir.x;
            if (t > 0) {
                float y = currentStart.y + currentDir.y * t;
                if (y > 0) {
                    if (wallHitX == -1 || t < (wallHitX - currentStart.x) / currentDir.x) {
                        wallHitX = 800 - BUBBLE_RADIUS;
                        wallHitY = y;
                    }
                }
            }
        }

        // En yakın çarpışmayı belirle
        int hitWall = 0;
        Vector2 finalHit = hitPoint;

        if (wallHitX != -1) {
            float wallDist = sqrtf(powf(wallHitX - currentStart.x, 2) + powf(wallHitY - currentStart.y, 2));
            float bubbleDist = sqrtf(powf(hitPoint.x - currentStart.x, 2) + powf(hitPoint.y - currentStart.y, 2));

            if (!hitBubble || wallDist < bubbleDist) {
                finalHit = (Vector2){ wallHitX, wallHitY };
                hitWall = 1;
                hitBubble = 0;
            }
        }

        // Segment bitiş noktasını ekle
        segments[(*segmentCount)++] = finalHit;

        // Balona çarptıysa dur
        if (hitBubble) {
            break;
        }

        // Duvara çarptıysa sekme hesapla
        if (hitWall) {
            currentStart = finalHit;
            currentDir.x = -currentDir.x; // X yönünü tersine çevir
            // Bir sonraki segment başlangıcı
            if (*segmentCount < 10) { // Array sınırını kontrol et
                segments[(*segmentCount)++] = currentStart;
            }
        }
        else {
            // Tavana çarptıysa dur
            break;
        }

        // Array sınırını kontrol et
        if (*segmentCount >= 9) break; // 10 elemanlı array için
    }
}

// Noktalı çizgi çizme yardımcı fonksiyonu
static void DrawDottedLine(Vector2 start, Vector2 end, Color color) {
    float totalDist = sqrtf(powf(end.x - start.x, 2) + powf(end.y - start.y, 2));
    int dotCount = (int)(totalDist / 15.0f); // Her 15 pikselde bir nokta

    if (dotCount == 0) return;

    for (int i = 1; i <= dotCount; i++) {
        float t = (float)i / dotCount;
        Vector2 dotPos = {
            start.x + (end.x - start.x) * t,
            start.y + (end.y - start.y) * t
        };
        // Noktaların boyutunu mesafeye göre ayarla
        float dotSize = 4.0f * (1.0f - t * 0.3f); // Uzaktaki noktalar daha küçük
        DrawCircleV(dotPos, dotSize, color);
    }
}

// Oyuncu ve nişangahı çizer
void DrawPlayer(const Player* player, const BubbleGrid* grid) {
    // Nişan çizgisi için yön vektörü
    float rad = player->angle * (PI / 180.0f);
    Vector2 start = player->pos;
    Vector2 dir = { cosf(rad), -sinf(rad) };

    // Tam yolu hesapla
    Vector2 segments[10]; // Maksimum 10 nokta (başlangıç + sekmeler + bitiş)
    int segmentCount;
    CalculateAimPath(start, dir, grid, segments, &segmentCount);

    // Her segment için noktalı çizgi çiz
    Color aimColor = GetColor(player->bubble.color);
    aimColor.a = 180; // Biraz şeffaf yap

    for (int i = 0; i < segmentCount - 1; i++) {
        DrawDottedLine(segments[i], segments[i + 1], aimColor);
    }

    // Fırlatılacak balon
    if (!player->shooting) {
        DrawCircleV(player->pos, BUBBLE_RADIUS, LIGHTGRAY);
        DrawCircleV(player->pos, BUBBLE_RADIUS - 2, GetColor(player->bubble.color));
    }
    else {
        DrawCircleV(player->bubble.pos, BUBBLE_RADIUS, GetColor(player->bubble.color));
    }
}

// Oyuncu temizleme
void UnloadPlayer(Player* player) {
    // Şu an için temizlenecek bir şey yok
}