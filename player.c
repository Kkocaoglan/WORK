#define _CRT_SECURE_NO_WARNINGS
#include "player.h"
#include "raylib.h"
#include "bubble.h"
#include <math.h>
#include <stdio.h>

// Sabitler
#define PLAYER_Y 550
#define MIN_ANGLE 0.0f
#define MAX_ANGLE 180.0f
#define BUBBLE_SPEED 8.0f
#define BOMB_BUTTON_SIZE 40
#define BOMB_BUTTON_MARGIN 60
#define MAX_MOVES 80

// Oyuncu başlatılır
void InitPlayer(Player* player) {
    player->pos = (Vector2){ 400, PLAYER_Y };
    player->angle = 90.0f;
    player->shooting = 0;
    player->bubble.active = 1;

    // İlk topun rengini seç (siyah hariç)
    int availableColors[BUBBLE_COLORS - 1];  // Siyah hariç renkler için dizi
    int colorCount = 0;

    // Siyah hariç tüm renkleri diziye ekle
    for (int i = 0; i < BUBBLE_COLORS; i++) {
        if (i != BUBBLE_NONE) {
            availableColors[colorCount++] = i;
        }
    }

    // Rastgele bir renk seç
    int randomIndex = GetRandomValue(0, colorCount - 1);
    player->bubble.color = (BubbleColor)availableColors[randomIndex];

    player->bubble.pos = player->pos;
    player->bombCount = 3;
    player->canUseBomb = true;
    player->score = 0;
    player->bombActive = false;
}

// Yeni balon hazırla
void NextBubble(Player* player, const BubbleGrid* grid) {
    player->shooting = 0;
    player->bubble.active = 1;

    // Ekranda kalan top sayısını hesapla
    int remainingBubblesOnScreen = 0;
    int colorCounts[BUBBLE_COLORS] = { 0 };
    int availableColors[BUBBLE_COLORS - 1] = { 0 };  // Siyah hariç renkler
    int availableColorCount = 0;

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
        // Mevcut renkleri bul
        for (int i = 0; i < BUBBLE_COLORS; i++) {
            if (i != BUBBLE_NONE && colorCounts[i] > 0) {
                availableColors[availableColorCount++] = i;
            }
        }

        // Eğer kullanılabilir renk varsa, onlardan birini seç
        if (availableColorCount > 0) {
            int randomIndex = GetRandomValue(0, availableColorCount - 1);
            player->bubble.color = (BubbleColor)availableColors[randomIndex];
        }
        else {
            // Hiç renk kalmadıysa varsayılan olarak kırmızı rengi seç
            player->bubble.color = BUBBLE_RED;
        }
    }
    else {
        // Normal rastgele renk seçimi (siyah hariç)
        int availableColors[BUBBLE_COLORS - 1];  // Siyah hariç renkler için dizi
        int colorCount = 0;

        // Siyah hariç tüm renkleri diziye ekle
        for (int i = 0; i < BUBBLE_COLORS; i++) {
            if (i != BUBBLE_NONE) {
                availableColors[colorCount++] = i;
            }
        }

        // Rastgele bir renk seç
        int randomIndex = GetRandomValue(0, colorCount - 1);
        player->bubble.color = (BubbleColor)availableColors[randomIndex];
    }

    player->bubble.pos = player->pos;
    player->velocity = (Vector2){ 0, 0 };
}

// Mouse pozisyonundan açı hesapla
float CalculateAngleFromMouse(Vector2 mousePos, Vector2 playerPos) {
    float dx = mousePos.x - playerPos.x;
    float dy = playerPos.y - mousePos.y;
    float angle = atan2f(dy, dx) * (180.0f / PI);

    if (angle < MIN_ANGLE) angle = MIN_ANGLE;
    if (angle > MAX_ANGLE) angle = MAX_ANGLE;

    return angle;
}

// Bomba kullan
void UseBomb(Player* player) {
    if (player->bombCount > 0 && player->canUseBomb) {
        // Eğer top siyah değilse, bombayı kullan
        if (player->bubble.color != BUBBLE_NONE) {
            player->bombCount--;
            player->bubble.color = BUBBLE_NONE;  // Mevcut topun rengini siyah yap
        }
    }
}

// Bomba patlaması ve yakındaki topları yok etme
void HandleBombExplosion(Player* player, BubbleGrid* grid, Vector2 hitPos) {
    const float EXPLOSION_RADIUS = BUBBLE_RADIUS * 3.0f;

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                Vector2 bubblePos = grid->bubbles[r][c].pos;
                float dx = bubblePos.x - hitPos.x;
                float dy = bubblePos.y - hitPos.y;
                float distance = sqrtf(dx * dx + dy * dy);

                if (distance <= EXPLOSION_RADIUS) {
                    grid->bubbles[r][c].active = 0;
                    player->score += 10;
                }
            }
        }
    }
}

// Bağlı topları bul
static void FindConnectedBubbles(BubbleGrid* grid, int startRow, int startCol, BubbleColor color,
    bool visited[GRID_ROWS][GRID_COLS], int* matchCount, int matches[][2]) {
    if (visited[startRow][startCol] ||
        !grid->bubbles[startRow][startCol].active ||
        grid->bubbles[startRow][startCol].color != color) {
        return;
    }

    visited[startRow][startCol] = true;
    matches[*matchCount][0] = startRow;
    matches[*matchCount][1] = startCol;
    (*matchCount)++;

    int directions[4][2] = {
        {-1, 0},  // yukarı
        {1, 0},   // aşağı
        {0, -1},  // sol
        {0, 1}    // sağ
    };

    for (int i = 0; i < 4; i++) {
        int nr = startRow + directions[i][0];
        int nc = startCol + directions[i][1];

        if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS) {
            FindConnectedBubbles(grid, nr, nc, color, visited, matchCount, matches);
        }
    }
}

// Bomba butonunu çiz
void DrawBombButton(const Player* player) {
    Vector2 buttonPos = {
        player->pos.x + BUBBLE_RADIUS + BOMB_BUTTON_MARGIN,
        player->pos.y
    };

    Color buttonColor = player->bombCount > 0 ? RED : GRAY;
    DrawCircleV(buttonPos, BOMB_BUTTON_SIZE / 2, buttonColor);
    DrawCircleLinesV(buttonPos, BOMB_BUTTON_SIZE / 2, BLACK);

    float iconSize = BOMB_BUTTON_SIZE / 3;
    DrawLineEx(
        (Vector2) {
        buttonPos.x - iconSize, buttonPos.y - iconSize
    },
        (Vector2) {
        buttonPos.x + iconSize, buttonPos.y + iconSize
    },
        3, WHITE
    );
    DrawLineEx(
        (Vector2) {
        buttonPos.x + iconSize, buttonPos.y - iconSize
    },
        (Vector2) {
        buttonPos.x - iconSize, buttonPos.y + iconSize
    },
        3, WHITE
    );

    char bombText[8];
    sprintf(bombText, "%d", player->bombCount);
    int textWidth = MeasureText(bombText, 20);
    DrawText(bombText,
        buttonPos.x - textWidth / 2,
        buttonPos.y + BOMB_BUTTON_SIZE / 2 + 5,
        20, WHITE);
}

// Oyuncu kontrolü ve balon fırlatma
void UpdatePlayer(Player* player, BubbleGrid* grid) {
    if (!player->shooting) {
        Vector2 mousePos = GetMousePosition();
        float mouseAngle = CalculateAngleFromMouse(mousePos, player->pos);

        float angleChange = 0.0f;
        if (IsKeyDown(KEY_LEFT)) angleChange -= 2.0f;
        if (IsKeyDown(KEY_RIGHT)) angleChange += 2.0f;

        if (angleChange != 0.0f) {
            player->angle += angleChange;
        }
        else {
            float angleDiff = mouseAngle - player->angle;
            if (fabsf(angleDiff) > 180.0f) {
                if (angleDiff > 0) angleDiff -= 360.0f;
                else angleDiff += 360.0f;
            }
            player->angle += angleDiff * 0.2f;
        }

        if (player->angle < MIN_ANGLE) player->angle = MIN_ANGLE;
        if (player->angle > MAX_ANGLE) player->angle = MAX_ANGLE;

        Vector2 buttonPos = {
            player->pos.x + BUBBLE_RADIUS + BOMB_BUTTON_MARGIN,
            player->pos.y
        };

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Rectangle bombButton = {
                buttonPos.x - BOMB_BUTTON_SIZE / 2,
                buttonPos.y - BOMB_BUTTON_SIZE / 2,
                BOMB_BUTTON_SIZE,
                BOMB_BUTTON_SIZE
            };

            if (CheckCollisionPointRec(mousePos, bombButton)) {
                UseBomb(player);
                return;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)) {
            player->shooting = 1;
            float rad = player->angle * (PI / 180.0f);
            player->velocity.x = cosf(rad) * BUBBLE_SPEED;
            player->velocity.y = -sinf(rad) * BUBBLE_SPEED;
            player->bubble.pos = player->pos;
        }
    }
    else {
        player->bubble.pos.x += player->velocity.x;
        player->bubble.pos.y += player->velocity.y;

        if (player->bubble.pos.x < BUBBLE_RADIUS) {
            player->bubble.pos.x = BUBBLE_RADIUS;
            player->velocity.x *= -1;
        }
        if (player->bubble.pos.x > 800 - BUBBLE_RADIUS) {
            player->bubble.pos.x = 800 - BUBBLE_RADIUS;
            player->velocity.x *= -1;
        }
        // Çarpışma ve patlatma mantığı artık sadece collision.c'de olacak!
        // Burada sadece balonun hareketi var.
    }
}

// Nişan çizgisi için yardımcı fonksiyonlar
static int CheckRayBubbleCollision(Vector2 start, Vector2 dir, const BubbleGrid* grid, Vector2* hitPoint) {
    float step = 2.0f;
    Vector2 current = start;
    float maxDistance = 1000.0f;
    float traveled = 0.0f;

    while (traveled < maxDistance && current.y > 0) {
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (grid->bubbles[r][c].active) {
                    float dx = current.x - grid->bubbles[r][c].pos.x;
                    float dy = current.y - grid->bubbles[r][c].pos.y;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist < BUBBLE_RADIUS * 2) {
                        *hitPoint = current;
                        return 1;
                    }
                }
            }
        }

        current.x += dir.x * step;
        current.y += dir.y * step;
        traveled += step;
    }

    *hitPoint = current;
    return 0;
}

static void CalculateAimPath(Vector2 start, Vector2 initialDir, const BubbleGrid* grid,
    Vector2* segments, int* segmentCount) {
    Vector2 currentStart = start;
    Vector2 currentDir = initialDir;
    *segmentCount = 0;
    int maxBounces = 3;

    segments[(*segmentCount)++] = currentStart;

    for (int bounce = 0; bounce <= maxBounces; bounce++) {
        Vector2 hitPoint;
        int hitBubble = CheckRayBubbleCollision(currentStart, currentDir, grid, &hitPoint);

        float wallHitX = -1;
        float wallHitY = -1;

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

        segments[(*segmentCount)++] = finalHit;

        if (hitBubble) {
            break;
        }

        if (hitWall) {
            currentStart = finalHit;
            currentDir.x = -currentDir.x;
            if (*segmentCount < 10) {
                segments[(*segmentCount)++] = currentStart;
            }
        }
        else {
            break;
        }

        if (*segmentCount >= 9) break;
    }
}

static void DrawDottedLine(Vector2 start, Vector2 end, Color color) {
    float totalDist = sqrtf(powf(end.x - start.x, 2) + powf(end.y - start.y, 2));
    int dotCount = (int)(totalDist / 15.0f);

    if (dotCount == 0) return;

    for (int i = 1; i <= dotCount; i++) {
        float t = (float)i / dotCount;
        Vector2 dotPos = {
            start.x + (end.x - start.x) * t,
            start.y + (end.y - start.y) * t
        };
        float dotSize = 4.0f * (1.0f - t * 0.3f);
        DrawCircleV(dotPos, dotSize, color);
    }
}

// Oyuncu ve nişangahı çizer
void DrawPlayer(const Player* player, const BubbleGrid* grid) {
    float rad = player->angle * (PI / 180.0f);
    Vector2 start = player->pos;
    Vector2 dir = { cosf(rad), -sinf(rad) };

    Vector2 segments[10];
    int segmentCount;
    CalculateAimPath(start, dir, grid, segments, &segmentCount);

    Color aimColor = GetColor(player->bubble.color);
    aimColor.a = 180;

    for (int i = 0; i < segmentCount - 1; i++) {
        DrawDottedLine(segments[i], segments[i + 1], aimColor);
    }

    if (!player->shooting) {
        DrawCircleV(player->pos, BUBBLE_RADIUS, LIGHTGRAY);
        DrawCircleV(player->pos, BUBBLE_RADIUS - 2, GetColor(player->bubble.color));
    }
    else {
        DrawCircleV(player->bubble.pos, BUBBLE_RADIUS, GetColor(player->bubble.color));
    }

    DrawBombButton(player);
}

// Oyuncu temizleme
void UnloadPlayer(Player* player) {
    // Şu an için temizlenecek bir şey yok
}