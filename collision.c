#include "collision.h"
#include <math.h>
#include <string.h>
#include <stdio.h>



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

// Yardımcı: Ekran koordinatından grid koordinatına yaklaşık dönüşüm
static void FindNearestGridCell(BubbleGrid* grid, Vector2 pos, int* outRow, int* outCol) {
    int bestR = 0, bestC = 0;
    float minDist = 1e9f;
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

// Balonu gridde uygun yere yerleştirir, çarpışan balonun komşu boş hücresine veya tavana
void PlaceBubble(Bubble* bubble, BubbleGrid* grid, int* outRow, int* outCol) {
    int baseR = 0, baseC = 0;
    // Eğer tavana çarptıysa, en üst satırda en yakın boş hücreye yerleştir
    if (bubble->pos.y < grid->bubbles[0][0].pos.y + BUBBLE_RADIUS) {
        float minDist = 1e9f;
        int bestC = 0;
        for (int c = 0; c < GRID_COLS; c++) {
            if (!grid->bubbles[0][c].active) {
                float dx = bubble->pos.x - grid->bubbles[0][c].pos.x;
                float dist = dx * dx;
                if (dist < minDist) {
                    minDist = dist;
                    bestC = c;
                }
            }
        }
        grid->bubbles[0][bestC].active = 1;
        grid->bubbles[0][bestC].color = bubble->color;
        if (outRow) *outRow = 0;
        if (outCol) *outCol = bestC;
        return;
    }
    // Tavana çarpmadıysa, çarpışan balona en yakın grid hücresini bul
    FindNearestGridCell(grid, bubble->pos, &baseR, &baseC);
    // 6 komşuya bak, en yakın boş olanı bul
    int dr[6] = { -1, -1, 0, 0, 1, 1 };
    int dc_even[6] = { -1, 0, -1, 1, -1, 0 };
    int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };
    int bestR = -1, bestC = -1;
    float minDist = 1e9f;
    for (int d = 0; d < 6; d++) {
        int nr = baseR + dr[d];
        int nc = baseC + ((baseR % 2 == 0) ? dc_even[d] : dc_odd[d]);
        if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS && !grid->bubbles[nr][nc].active) {
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
    // Eğer hiç boş komşu yoksa, base hücresine yerleştir (son çare)
    if (bestR == -1 || bestC == -1) {
        bestR = baseR;
        bestC = baseC;
    }
    grid->bubbles[bestR][bestC].active = 1;
    grid->bubbles[bestR][bestC].color = bubble->color;
    if (outRow) *outRow = bestR;
    if (outCol) *outCol = bestC;
}

// DFS ile bağlantılı aynı renkteki balonları bulup patlatır
static void dfs(BubbleGrid* grid, int r, int c, BubbleColor color, int visited[GRID_ROWS][GRID_COLS], int* count) {
    // Sınır kontrolü
    if (r < 0 || r >= GRID_ROWS || c < 0 || c >= GRID_COLS) return;
    // Aktiflik, renk ve ziyaret kontrolü
    if (!grid->bubbles[r][c].active || grid->bubbles[r][c].color != color || visited[r][c]) return;
    // Ziyaret edildi olarak işaretle
    visited[r][c] = 1;
    (*count)++;
    // Altıgen grid'de 6 komşu yönü
    int dr[6] = { -1, -1, 0, 0, 1, 1 };
    int dc_even[6] = { -1, 0, -1, 1, -1, 0 }; // çift satır
    int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };   // tek satır
    // Komşuları kontrol et
    for (int d = 0; d < 6; d++) {
        int nr = r + dr[d];
        int nc = c + ((r % 2 == 0) ? dc_even[d] : dc_odd[d]);
        // Geçerli bir komşu ise DFS'i devam ettir
        if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS) {
            dfs(grid, nr, nc, color, visited, count);
        }
    }
}

int PopConnectedBubbles(BubbleGrid* grid, int row, int col) {
    if (!grid->bubbles[row][col].active) return 0;
    printf("PopConnectedBubbles başlatıldı: row=%d, col=%d, color=%d\n", row, col, grid->bubbles[row][col].color);
    int visited[GRID_ROWS][GRID_COLS] = { 0 };
    int count = 0;
    BubbleColor color = grid->bubbles[row][col].color;
    dfs(grid, row, col, color, visited, &count);
    printf("Bulunan bağlantılı balon sayısı: %d\n", count);
    if (count >= 3) {
        // Patlayan balonları göster
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                if (visited[r][c]) {
                    printf("Patlatılıyor: row=%d, col=%d\n", r, c);
                    grid->bubbles[r][c].active = 0;
                }
            }
        }
        return count;
    }
    return 0;
}

// Tavana bağlı olmayan balonları BFS ile bulup düşürür
int DropFloatingBubbles(BubbleGrid* grid) {
    int visited[GRID_ROWS][GRID_COLS] = { 0 };
    // Tavana bağlı olanları işaretle
    for (int c = 0; c < GRID_COLS; c++) {
        if (grid->bubbles[0][c].active) {
            int queue[GRID_ROWS * GRID_COLS][2];
            int front = 0, back = 0;
            queue[back][0] = 0; queue[back][1] = c; back++;
            while (front < back) {
                int r = queue[front][0], cc = queue[front][1]; front++;
                if (visited[r][cc]) continue;
                visited[r][cc] = 1;
                // Altıgen gridde 6 komşu
                int dr[6] = { -1, -1, 0, 0, 1, 1 };
                int dc_even[6] = { -1, 0, -1, 1, -1, 0 }; // çift satır
                int dc_odd[6] = { 0, 1, -1, 1, 0, 1 };   // tek satır
                for (int d = 0; d < 6; d++) {
                    int nr = r + dr[d];
                    int nc = cc + ((r % 2 == 0) ? dc_even[d] : dc_odd[d]);
                    if (nr >= 0 && nr < GRID_ROWS && nc >= 0 && nc < GRID_COLS && grid->bubbles[nr][nc].active && !visited[nr][nc]) {
                        if (back < GRID_ROWS * GRID_COLS) {
                            queue[back][0] = nr; queue[back][1] = nc; back++;
                        }
                    }
                }
            }
        }
    }
    // Bağlantısızları düşür
    int dropped = 0;
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active && !visited[r][c]) {
                grid->bubbles[r][c].active = 0;
                dropped++;
            }
        }
    }
    return dropped;
}