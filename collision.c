#include "collision.h"
#include "bubble.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

// Global patlama kontrolü - çoklu patlamayı önlemek için
static bool explosionInProgress = false;

// Global skor değişkeni
static int totalScore = 0;

// Fırlatılan balonun griddeki balonlara çarpıp çarpmadığını kontrol eder
int CheckBubbleCollision(const BubbleGrid* grid, const Bubble* bubble) {
    // Tavana çarpma kontrolü
    if (bubble->pos.y <= BUBBLE_RADIUS) {
        return 1;
    }

    // Griddeki balonlarla çarpışma kontrolü
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                float dx = bubble->pos.x - grid->bubbles[r][c].pos.x;
                float dy = bubble->pos.y - grid->bubbles[r][c].pos.y;
                float dist = sqrtf(dx * dx + dy * dy);

                if (dist < BUBBLE_RADIUS * 2) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

// Yardımcı: Ekran koordinatından grid koordinatına yaklaşık dönüşüm
static void FindNearestGridCell(BubbleGrid* grid, Vector2 pos, int* outRow, int* outCol) {
    int bestR = 0, bestC = 0;
    float minDist = 1e9f;
    float hex_width = BUBBLE_RADIUS * 2;  // Altıgenin genişliği
    float hex_height = BUBBLE_RADIUS * 1.73f;  // Altıgenin yüksekliği

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            float dx = pos.x - grid->bubbles[r][c].pos.x;
            float dy = pos.y - grid->bubbles[r][c].pos.y;
            float dist = dx * dx + dy * dy;
            if (dist < minDist) {
                minDist = dist;
                bestR = r;
                bestC = c;
            }
        }
    }
    if (outRow) *outRow = bestR;
    if (outCol) *outCol = bestC;
}

// Balonu gridde uygun yere yerleştirir
void PlaceBubble(Bubble* bubble, BubbleGrid* grid, int* outRow, int* outCol) {
    printf("\n=== PlaceBubble Başladı ===\n");
    printf("Fırlatılan balon pozisyonu: x=%.2f, y=%.2f, renk=%d\n",
        bubble->pos.x, bubble->pos.y, bubble->color);

    float hex_width = BUBBLE_RADIUS * 2;  // Altıgenin genişliği
    float hex_height = BUBBLE_RADIUS * 1.73f;  // Altıgenin yüksekliği

    // En yakın grid hücresini bul
    int row = 0;
    int col = 0;
    float minDist = 999999.0f;

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (!grid->bubbles[r][c].active) {
                float dx = bubble->pos.x - grid->bubbles[r][c].pos.x;
                float dy = bubble->pos.y - grid->bubbles[r][c].pos.y;
                float dist = sqrtf(dx * dx + dy * dy);

                if (dist < minDist) {
                    minDist = dist;
                    row = r;
                    col = c;
                }
            }
        }
    }

    // Balonu yerleştir - tam olarak grid hücresinin merkezine
    grid->bubbles[row][col].active = 1;
    grid->bubbles[row][col].color = bubble->color;
    grid->bubbles[row][col].pos = grid->bubbles[row][col].pos;  // Grid hücresinin merkezi

    // Çıktı değerlerini ayarla
    *outRow = row;
    *outCol = col;

    // Eğer siyah balon yerleştirildiyse, patlama etkisini uygula
    if (bubble->color == BUBBLE_NONE) {
        // Hexagonal grid komşuluk tablosu
        int dr[6] = { -1, -1, 0, 0, 1, 1 };
        int dc_even[6] = { -1, 0, -1, 1, -1, 0 };  // Çift satırlar için
        int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };     // Tek satırlar için

        // 6 komşuyu patlat
        for (int d = 0; d < 6; d++) {
            int newRow = row + dr[d];
            int newCol = col + ((row % 2 == 0) ? dc_even[d] : dc_odd[d]);
            if (newRow >= 0 && newRow < GRID_ROWS && newCol >= 0 && newCol < GRID_COLS && grid->bubbles[newRow][newCol].active) {
                grid->bubbles[newRow][newCol].active = 0;
            }
        }
        // Kendisini de patlat
        grid->bubbles[row][col].active = 0;

        // Patlatma sonucu süzülen topları bul ve patlat
        bool connected[GRID_ROWS][GRID_COLS] = { false };
        int queue[GRID_ROWS * GRID_COLS][2];
        int front = 0, back = 0;

        // Tavan satırından başla
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[0][c].active) {
                connected[0][c] = true;
                queue[back][0] = 0;
                queue[back][1] = c;
                back++;
            }
        }

        // BFS ile bağlı topları işaretle
        while (front < back) {
            int r = queue[front][0];
            int cc = queue[front][1];
            front++;

            for (int d = 0; d < 6; d++) {
                int nr = r + dr[d];
                int nc = cc + ((r % 2 == 0) ? dc_even[d] : dc_odd[d]);

                if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS &&
                    grid->bubbles[nr][nc].active && !connected[nr][nc]) {
                    connected[nr][nc] = true;
                    queue[back][0] = nr;
                    queue[back][1] = nc;
                    back++;
                }
            }
        }

        // Tavana bağlı olmayanları patlat
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (grid->bubbles[r][c].active && !connected[r][c]) {
                    grid->bubbles[r][c].active = 0;
                }
            }
        }
    }

    printf("=== PlaceBubble Bitti ===\n\n");
}

// Güvenlik kontrolü fonksiyonu
static int isValidExplosion(BubbleGrid* grid, int row, int col) {
    // Koordinat kontrolü
    if (row < 0 || row >= GRID_ROWS || col < 0 || col >= GRID_COLS) {
        printf("HATA: Geçersiz koordinat row=%d, col=%d\n", row, col);
        return 0;
    }

    // Aktif balon var mı?
    if (!grid->bubbles[row][col].active) {
        printf("HATA: Aktif olmayan balonda patlama denemesi row=%d, col=%d\n", row, col);
        return 0;
    }

    return 1;
}

// GÜÇLENDIRILMIŞ DFS algoritması - Aynı balonu birden fazla ziyaret etmeyi önler
static void dfs(BubbleGrid* grid, int r, int c, BubbleColor color, int visited[GRID_ROWS][GRID_COLS], int* count) {
    // Sınır kontrolü
    if (r < 0 || r >= GRID_ROWS || c < 0 || c >= GRID_COLS) return;

    // ÖNEMLİ: Zaten ziyaret edildi mi kontrolü EN BAŞTA olmalı
    if (visited[r][c]) return;

    // Aktif değil veya farklı renk mi?
    if (!grid->bubbles[r][c].active || grid->bubbles[r][c].color != color) return;

    // Bu hücreyi HEMEN işaretle - bu çok önemli!
    visited[r][c] = 1;
    (*count)++;

    printf("DFS: Balon işaretlendi: row=%d, col=%d, renk=%d, toplam=%d\n", r, c, color, *count);

    // Hexagonal grid komşuluk tablosu
    int dr[6] = { -1, -1, 0, 0, 1, 1 };
    int dc_even[6] = { -1, 0, -1, 1, -1, 0 };  // Çift satırlar için
    int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };     // Tek satırlar için

    // 6 komşuyu ziyaret et
    for (int d = 0; d < 6; d++) {
        int nr = r + dr[d];
        int nc = c + ((r % 2 == 0) ? dc_even[d] : dc_odd[d]);
        dfs(grid, nr, nc, color, visited, count);
    }
}

// Bağlı topları bul
void FindConnectedBubbles(BubbleGrid* grid, int row, int col, BubbleColor color, bool visited[GRID_ROWS][GRID_COLS], int* matchCount, int matches[][2]) {
    // Sınır kontrolü
    if (row < 0 || row >= GRID_ROWS || col < 0 || col >= GRID_COLS) return;

    // Zaten ziyaret edildi mi kontrolü
    if (visited[row][col]) return;

    // Aktif değil veya farklı renk mi?
    if (!grid->bubbles[row][col].active || grid->bubbles[row][col].color != color) return;

    // Bu hücreyi işaretle
    visited[row][col] = true;
    matches[*matchCount][0] = row;
    matches[*matchCount][1] = col;
    (*matchCount)++;

    // Hexagonal grid komşuluk tablosu
    int dr[6] = { -1, -1, 0, 0, 1, 1 };
    int dc_even[6] = { -1, 0, -1, 1, -1, 0 };  // Çift satırlar için
    int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };     // Tek satırlar için

    // 6 komşuyu ziyaret et
    for (int d = 0; d < 6; d++) {
        int nr = row + dr[d];
        int nc = col + ((row % 2 == 0) ? dc_even[d] : dc_odd[d]);
        FindConnectedBubbles(grid, nr, nc, color, visited, matchCount, matches);
    }
}

// Bağlı topları patlat
int PopConnectedBubbles(BubbleGrid* grid, int row, int col) {
    if (!grid->bubbles[row][col].active) return 0;

    BubbleColor color = grid->bubbles[row][col].color;
    bool visited[GRID_ROWS][GRID_COLS] = { false };
    int matches[GRID_ROWS * GRID_COLS][2];
    int matchCount = 0;
    int totalPopped = 0;

    // Bağlı topları bul
    FindConnectedBubbles(grid, row, col, color, visited, &matchCount, matches);

    // Eğer 3 veya daha fazla bağlı top varsa patlat
    if (matchCount >= 3) {
        // 1. Eşleşen topları patlat
        for (int i = 0; i < matchCount; i++) {
            int r = matches[i][0];
            int c = matches[i][1];
            if (grid->bubbles[r][c].active) {
                grid->bubbles[r][c].active = false;
                totalPopped++;
            }
        }

        // 2. Tavana bağlı olanları işaretle (BFS)
        bool connected[GRID_ROWS][GRID_COLS] = { false };
        int queue[GRID_ROWS * GRID_COLS][2];
        int front = 0, back = 0;

        // Tavan satırından başla
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[0][c].active) {
                connected[0][c] = true;
                queue[back][0] = 0;
                queue[back][1] = c;
                back++;
            }
        }

        // BFS ile bağlı topları işaretle
        int dr[6] = { -1, -1, 0, 0, 1, 1 };
        int dc_even[6] = { -1, 0, -1, 1, -1, 0 };
        int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };

        while (front < back) {
            int r = queue[front][0];
            int cc = queue[front][1];
            front++;

            for (int d = 0; d < 6; d++) {
                int nr = r + dr[d];
                int nc = cc + ((r % 2 == 0) ? dc_even[d] : dc_odd[d]);

                if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS &&
                    grid->bubbles[nr][nc].active && !connected[nr][nc]) {
                    connected[nr][nc] = true;
                    queue[back][0] = nr;
                    queue[back][1] = nc;
                    back++;
                }
            }
        }

        // 3. Tavana bağlı olmayanları patlat
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (grid->bubbles[r][c].active && !connected[r][c]) {
                    grid->bubbles[r][c].active = false;
                    totalPopped++;
                }
            }
        }
    }

    return totalPopped;  // Toplam patlatılan top sayısını döndür
}

// Patlama durumunu sıfırlama fonksiyonu (yeni top fırlatırken kullanın)
void ResetExplosionState(void) {
    explosionInProgress = false;
    printf("Patlama durumu sıfırlandı.\n");
}

// Havada kalan balonları düşürür
int DropFloatingBubbles(BubbleGrid* grid) {
    printf("\n=== DropFloatingBubbles Başladı ===\n");

    int visited[GRID_ROWS][GRID_COLS];
    memset(visited, 0, sizeof(visited));

    // Üst satırdaki balonlardan başlayarak bağlı balonları işaretle
    int queue[GRID_ROWS * GRID_COLS][2];
    int front = 0, back = 0;

    // Tavan satırından başla
    for (int c = 0; c < GRID_COLS; c++) {
        if (grid->bubbles[0][c].active && !visited[0][c]) {
            queue[back][0] = 0;
            queue[back][1] = c;
            back++;
            visited[0][c] = 1;
        }
    }

    // BFS işlemi
    while (front < back) {
        int r = queue[front][0];
        int cc = queue[front][1];
        front++;

        // Hexagonal grid komşuluk
        int dr[6] = { -1, -1, 0, 0, 1, 1 };
        int dc_even[6] = { -1, 0, -1, 1, -1, 0 };
        int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };

        for (int d = 0; d < 6; d++) {
            int nr = r + dr[d];
            int nc = cc + ((r % 2 == 0) ? dc_even[d] : dc_odd[d]);

            if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS &&
                grid->bubbles[nr][nc].active && !visited[nr][nc]) {

                visited[nr][nc] = 1;
                if (back < GRID_ROWS * GRID_COLS) {
                    queue[back][0] = nr;
                    queue[back][1] = nc;
                    back++;
                }
            }
        }
    }

    // İşaretlenmemiş balonları düşür
    int dropped = 0;
    printf("Düşürülecek balonlar:\n");
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active && !visited[r][c]) {
                printf("  -> row=%d, col=%d, renk=%d\n", r, c, grid->bubbles[r][c].color);
                grid->bubbles[r][c].active = 0;
                grid->bubbles[r][c].color = 0;
                dropped++;
            }
        }
    }

    printf("Toplam düşürülen balon sayısı: %d\n", dropped);
    printf("=== DropFloatingBubbles Bitti ===\n\n");
    return dropped;
}