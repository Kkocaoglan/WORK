#include "collision.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

// Global patlama kontrolü - çoklu patlamayı önlemek için
static int explosionInProgress = 0;

// Fırlatılan balonun griddeki balonlara çarpıp çarpmadığını kontrol eder
int CheckBubbleCollision(const Bubble* bubble, const BubbleGrid* grid) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                float dx = bubble->pos.x - grid->bubbles[r][c].pos.x;
                float dy = bubble->pos.y - grid->bubbles[r][c].pos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < BUBBLE_RADIUS * 2 - 2) {
                    return 1;
                }
            }
        }
    }
    // Tavana çarptı mı?
    if (bubble->pos.y < 60) return 1;
    return 0;
}

// Balonu gridde uygun yere yerleştirir
void PlaceBubble(Bubble* bubble, BubbleGrid* grid, int* outRow, int* outCol) {
    printf("\n=== PlaceBubble Basladi ===\n");
    printf("Firlatilan balon pozisyonu: x=%.2f, y=%.2f, renk=%d\n",
        bubble->pos.x, bubble->pos.y, bubble->color);

    // Eger tavana carptiysa, en ust satirda en yakin bos hucreye yerlestir
    if (bubble->pos.y <= grid->bubbles[0][0].pos.y + BUBBLE_RADIUS) {
        printf("Tavana carpma tespit edildi\n");
        // Balonun x pozisyonuna en yakın grid hücresini bul
        int bestC = 0;
        float minDist = 1e9f;
        for (int c = 0; c < GRID_COLS; c++) {
            float dx = bubble->pos.x - grid->bubbles[0][c].pos.x;
            float dist = fabsf(dx);
            if (dist < minDist) {
                minDist = dist;
                bestC = c;
            }
        }
        // Eğer en yakın hücre boşsa oraya yerleştir
        if (!grid->bubbles[0][bestC].active) {
            grid->bubbles[0][bestC].active = 1;
            grid->bubbles[0][bestC].color = bubble->color;
            printf("Balon tavana yerlestirildi: row=0, col=%d\n", bestC);
            if (outRow) *outRow = 0;
            if (outCol) *outCol = bestC;
            printf("=== PlaceBubble Bitti ===\n\n");
            return;
        } else {
            // Yanındaki en yakın boş hücreyi bul
            int left = bestC - 1;
            int right = bestC + 1;
            int found = 0;
            for (int offset = 1; offset < GRID_COLS && !found; offset++) {
                if (left >= 0 && !grid->bubbles[0][left].active) {
                    bestC = left;
                    found = 1;
                } else if (right < GRID_COLS && !grid->bubbles[0][right].active) {
                    bestC = right;
                    found = 1;
                }
                left--;
                right++;
            }
            if (found) {
                grid->bubbles[0][bestC].active = 1;
                grid->bubbles[0][bestC].color = bubble->color;
                printf("Balon tavana yerlestirildi (komsuya): row=0, col=%d\n", bestC);
                if (outRow) *outRow = 0;
                if (outCol) *outCol = bestC;
                printf("=== PlaceBubble Bitti ===\n\n");
                return;
            } else {
                // Hiç boş yoksa, fallback: ilk sütuna koy
                grid->bubbles[0][0].active = 1;
                grid->bubbles[0][0].color = bubble->color;
                if (outRow) *outRow = 0;
                if (outCol) *outCol = 0;
                printf("Balon tavana fallback ile yerlestirildi: row=0, col=0\n");
                printf("=== PlaceBubble Bitti ===\n\n");
                return;
            }
        }
    }

    // 1. Carpisma noktasina en yakin aktif balonu bul
    int nearestR = -1, nearestC = -1;
    float minDist = 1e9f;
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                float dx = bubble->pos.x - grid->bubbles[r][c].pos.x;
                float dy = bubble->pos.y - grid->bubbles[r][c].pos.y;
                float dist = dx * dx + dy * dy;
                if (dist < minDist) {
                    minDist = dist;
                    nearestR = r;
                    nearestC = c;
                }
            }
        }
    }

    // 2. O balonun 6 komsusundan en yakin bos olani bul
    int dr[6] = { -1, -1, 0, 0, 1, 1 };
    int dc_even[6] = { -1, 0, -1, 1, -1, 0 };
    int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };
    int bestR = -1, bestC = -1;
    minDist = 1e9f;

    for (int d = 0; d < 6; d++) {
        int nr = nearestR + dr[d];
        int nc = nearestC + ((nearestR % 2 == 0) ? dc_even[d] : dc_odd[d]);
        if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS) {
            if (!grid->bubbles[nr][nc].active) {
                float dx = bubble->pos.x - grid->bubbles[nr][nc].pos.x;
                float dy = bubble->pos.y - grid->bubbles[nr][nc].pos.y;
                float dist = dx * dx + dy * dy;
                if (dist < minDist) {
                    minDist = dist;
                    bestR = nr;
                    bestC = nc;
                }
            }
        }
    }

    // 3. Eger uygun bir hucre bulunduysa, balonu oraya yerlestir
    if (bestR != -1 && bestC != -1) {
        grid->bubbles[bestR][bestC].active = 1;
        grid->bubbles[bestR][bestC].color = bubble->color;
        printf("Balon yerlestirildi: row=%d, col=%d, renk=%d\n", bestR, bestC, bubble->color);
        if (outRow) *outRow = bestR;
        if (outCol) *outCol = bestC;
    }
    else {
        printf("UYARI: Uygun komsu bos hucre bulunamadi!\n");
        // Fallback: En yakin bos hucreyi bul
        minDist = 1e9f;
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (!grid->bubbles[r][c].active) {
                    float dx = bubble->pos.x - grid->bubbles[r][c].pos.x;
                    float dy = bubble->pos.y - grid->bubbles[r][c].pos.y;
                    float dist = dx * dx + dy * dy;
                    if (dist < minDist) {
                        minDist = dist;
                        bestR = r;
                        bestC = c;
                    }
                }
            }
        }
        if (bestR != -1 && bestC != -1) {
            grid->bubbles[bestR][bestC].active = 1;
            grid->bubbles[bestR][bestC].color = bubble->color;
            printf("Fallback ile balon yerleştirildi: row=%d, col=%d\n", bestR, bestC);
            if (outRow) *outRow = bestR;
            if (outCol) *outCol = bestC;
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

// GÜVENLI PATLAMA FONKSİYONU - Çoklu patlamayı önler
int PopConnectedBubbles(BubbleGrid* grid, int row, int col) {
    printf("\n=== PopConnectedBubbles BAŞLADI ===\n");
    printf("Patlama isteği: row=%d, col=%d\n", row, col);

    // ÇOKLU PATLAMA KONTROLÜ
    if (explosionInProgress) {
        printf("UYARI: Patlama zaten devam ediyor! Yeni patlama engellendi.\n");
        printf("=== PopConnectedBubbles İPTAL EDİLDİ ===\n\n");
        return 0;
    }

    // Patlama başlıyor
    explosionInProgress = 1;

    // GEÇERLİLİK KONTROLÜ
    if (!isValidExplosion(grid, row, col)) {
        printf("GEÇERSİZ PATLAMA İSTEĞİ!\n");
        explosionInProgress = 0;  // Reset
        printf("=== PopConnectedBubbles BAŞARISIZ ===\n\n");
        return 0;
    }

    BubbleColor color = grid->bubbles[row][col].color;
    printf("Patlama rengi: %d\n", color);

    // Visited array'i tamamen temizle
    int visited[GRID_ROWS][GRID_COLS];
    memset(visited, 0, sizeof(visited));

    int count = 0;

    // DFS çalıştır - bağlı balonları say
    dfs(grid, row, col, color, visited, &count);

    printf("Bulunan bağlantılı balon sayısı: %d\n", count);

    // 3 veya daha fazla balon varsa patlat
    if (count >= 3) {
        printf("*** PATLAMA YAPILIYOR! ***\n");

        // Önce hangi balonların patlatılacağını detaylı logla
        printf("Patlatılacak balonlar:\n");
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (visited[r][c]) {
                    printf("  -> row=%d, col=%d, renk=%d\n",
                        r, c, grid->bubbles[r][c].color);
                }
            }
        }

        // Sonra güvenli şekilde patlat
        int actuallyPopped = 0;
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (visited[r][c] && grid->bubbles[r][c].active) {
                    grid->bubbles[r][c].active = 0;
                    grid->bubbles[r][c].color = 0;  // Ek güvenlik
                    actuallyPopped++;
                    printf("Patlatıldı: row=%d, col=%d\n", r, c);
                }
            }
        }

        printf("*** PATLAMA TAMAMLANDI! Toplam patlatılan: %d ***\n", actuallyPopped);
        explosionInProgress = 0;  // Patlama bitti
        printf("=== PopConnectedBubbles BAŞARILI ===\n\n");
        return actuallyPopped;
    }
    else {
        printf("Yetersiz balon sayısı (%d < 3), patlama yok.\n", count);
        explosionInProgress = 0;  // Reset
        printf("=== PopConnectedBubbles BAŞARISIZ ===\n\n");
        return 0;
    }
}

// Patlama durumunu sıfırlama fonksiyonu (yeni top fırlatırken kullanın)
void ResetExplosionState() {
    explosionInProgress = 0;
    printf("Patlama durumu sıfırlandı.\n");
}

// Tavana bağlı olmayan balonları BFS ile bulup düşürür
int DropFloatingBubbles(BubbleGrid* grid) {
    printf("\n=== DropFloatingBubbles Başladı ===\n");

    int visited[GRID_ROWS][GRID_COLS];
    memset(visited, 0, sizeof(visited));

    // BFS ile tavana bağlı olanları işaretle
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

    // Tavana bağlı olmayan balonları düşür
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