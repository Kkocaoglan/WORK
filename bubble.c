#include "bubble.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

// Parçacık yapısı
typedef struct {
    Vector2 pos;
    Vector2 velocity;
    Color color;
    float life;
    bool active;
} Particle;

#define MAX_PARTICLES 1000
static Particle particles[MAX_PARTICLES];

// Parçacık oluştur
void CreateExplosionParticle(BubbleGrid* grid, Vector2 pos, Color color) {
    // Boş parçacık bul
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].pos = pos;
            particles[i].color = color;
            particles[i].life = 1.0f;
            particles[i].active = true;

            // Rastgele yön ve hız
            float angle = (float)rand() / RAND_MAX * 2 * PI;
            float speed = 2.0f + (float)rand() / RAND_MAX * 3.0f;
            particles[i].velocity.x = cosf(angle) * speed;
            particles[i].velocity.y = sinf(angle) * speed;
            break;
        }
    }
}

// Parçacıkları güncelle
void UpdateParticles(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].pos.x += particles[i].velocity.x;
            particles[i].pos.y += particles[i].velocity.y;
            particles[i].life -= 0.02f;

            if (particles[i].life <= 0) {
                particles[i].active = false;
            }
        }
    }
}

// Parçacıkları çiz
void DrawParticles(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            Color particleColor = particles[i].color;
            particleColor.a = (unsigned char)(particles[i].life * 255);
            DrawCircleV(particles[i].pos, 2, particleColor);
        }
    }
}

// Balon rengine karşılık gelen Raylib Color
Color GetColor(BubbleColor color) {
    switch (color) {
    case BUBBLE_RED: return (Color) { 255, 0, 0, 255 };      // Kırmızı
    case BUBBLE_GREEN: return (Color) { 0, 255, 0, 255 };    // Yeşil
    case BUBBLE_BLUE: return (Color) { 0, 0, 255, 255 };     // Mavi
    case BUBBLE_YELLOW: return (Color) { 255, 255, 0, 255 }; // Sarı
    case BUBBLE_PURPLE: return (Color) { 128, 0, 128, 255 }; // Mor
    case BUBBLE_NONE: return (Color) { 0, 0, 0, 255 };       // Siyah
    default: return (Color) { 255, 255, 255, 255 };          // Varsayılan beyaz
    }
}

// Grid'i başlat
void InitBubbleGrid(BubbleGrid* grid) {
    float x_offset = BUBBLE_RADIUS;
    float y_offset = BUBBLE_RADIUS;
    float hex_width = BUBBLE_RADIUS * 2;  // Altıgenin genişliği
    float hex_height = BUBBLE_RADIUS * 1.73f;  // Altıgenin yüksekliği

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            grid->bubbles[r][c].active = (r < 5) ? 1 : 0;
            grid->bubbles[r][c].color = (r < 5) ? (BubbleColor)(rand() % BUBBLE_COLORS) : BUBBLE_NONE;
            float x = x_offset + c * hex_width + (r % 2) * BUBBLE_RADIUS;
            float y = y_offset + r * hex_height;
            grid->bubbles[r][c].pos = (Vector2){ x, y };
        }
    }
}

// Grid'i çiz
void DrawBubbleGrid(const BubbleGrid* grid) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                DrawCircleV(grid->bubbles[r][c].pos, BUBBLE_RADIUS, GetColor(grid->bubbles[r][c].color));
                DrawCircleLines(grid->bubbles[r][c].pos.x, grid->bubbles[r][c].pos.y, BUBBLE_RADIUS, DARKGRAY);
            }
        }
    }
}

// Balonu çiz
void DrawBubble(Bubble* bubble) {
    if (bubble->active) {
        DrawCircleV(bubble->pos, BUBBLE_RADIUS, GetColor(bubble->color));
        DrawCircleLines(bubble->pos.x, bubble->pos.y, BUBBLE_RADIUS, DARKGRAY);
    }
}

// Nişancıyı çiz
void DrawShooter(float angle, BubbleColor color) {
    Vector2 start = { 400, 550 };
    float rad = angle * (PI / 180.0f);
    Color bubbleColor = GetColor(color);

    // Noktalı çizgi için noktaları çiz
    for (int i = 0; i < 20; i++) {
        float distance = i * (BUBBLE_RADIUS * 1.5f);  // Noktalar arası mesafe balon boyutuna göre ayarlandı
        Vector2 dot = {
            start.x + cosf(rad) * distance,
            start.y - sinf(rad) * distance
        };

        // Ekran sınırlarını kontrol et
        if (dot.x < 0 || dot.x > 800 || dot.y < 0 || dot.y > 600) {
            // Köşeye çarpma durumunu göster
            if (dot.x < 0) {
                // Sol duvara çarpma
                float newAngle = PI - rad;
                float remainingDistance = distance - (dot.x / cosf(rad));
                Vector2 newDot = {
                    -dot.x,
                    dot.y - sinf(rad) * remainingDistance
                };
                DrawCircleV(newDot, BUBBLE_RADIUS / 5, bubbleColor);
            }
            else if (dot.x > 800) {
                // Sağ duvara çarpma
                float newAngle = PI - rad;
                float remainingDistance = distance - ((dot.x - 800) / cosf(rad));
                Vector2 newDot = {
                    1600 - dot.x,
                    dot.y - sinf(rad) * remainingDistance
                };
                DrawCircleV(newDot, BUBBLE_RADIUS / 5, bubbleColor);
            }
            break;
        }

        DrawCircleV(dot, BUBBLE_RADIUS / 5, bubbleColor);
    }

    // Ana ok çizgisi
    Vector2 end = {
        start.x + cosf(rad) * (BUBBLE_RADIUS * 2.5f),
        start.y - sinf(rad) * (BUBBLE_RADIUS * 2.5f)
    };
    DrawLineEx(start, end, BUBBLE_RADIUS / 5, bubbleColor);
}

// Grid'i temizle
void UnloadBubbleGrid(BubbleGrid* grid) {
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            grid->bubbles[row][col].active = 0;
        }
    }
}

int IsGridFull(const BubbleGrid* grid) {
    for (int c = 0; c < GRID_COLS; c++) {
        if (grid->bubbles[GRID_ROWS - 1][c].active) return 1;
    }
    return 0;
}

// Tüm balonları patlat
void ExplodeAllBubbles(BubbleGrid* grid) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                // Her balon için 4 parçacık oluştur
                Vector2 pos = grid->bubbles[r][c].pos;
                Color color = GetColor(grid->bubbles[r][c].color);

                // Farklı yönlerde parçacıklar
                for (int i = 0; i < 4; i++) {
                    float angle = i * (PI / 2);
                    Vector2 offset = {
                        cosf(angle) * BUBBLE_RADIUS * 0.5f,
                        sinf(angle) * BUBBLE_RADIUS * 0.5f
                    };
                    Vector2 particlePos = {
                        pos.x + offset.x,
                        pos.y + offset.y
                    };
                    CreateExplosionParticle(grid, particlePos, color);
                }

                grid->bubbles[r][c].active = false;
            }
        }
    }
}