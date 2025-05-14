#include "collision.h"
#include <math.h>
#include <string.h>

// Fırlatılan balonun griddeki balonlara çarpıp çarpmadığını kontrol eder
int CheckBubbleCollision(const Bubble *bubble, const BubbleGrid *grid) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                float dx = bubble->pos.x - grid->bubbles[r][c].pos.x;
                float dy = bubble->pos.y - grid->bubbles[r][c].pos.y;
                float dist = sqrtf(dx*dx + dy*dy);
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

// Balonu gridde uygun yere yerleştirir (en yakın boş hücreye)
void PlaceBubble(Bubble *bubble, BubbleGrid *grid, int *outRow, int *outCol) {
    int minR = 0, minC = 0;
    float minDist = 1e9f;
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (!grid->bubbles[r][c].active) {
                float dx = bubble->pos.x - grid->bubbles[r][c].pos.x;
                float dy = bubble->pos.y - grid->bubbles[r][c].pos.y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < minDist) {
                    minDist = dist;
                    minR = r;
                    minC = c;
                }
            }
        }
    }
    grid->bubbles[minR][minC].active = 1;
    grid->bubbles[minR][minC].color = bubble->color;
    if (outRow) *outRow = minR;
    if (outCol) *outCol = minC;
}

// DFS ile bağlantılı aynı renkteki balonları bulup patlatır
static void dfs(BubbleGrid *grid, int r, int c, BubbleColor color, int visited[GRID_ROWS][GRID_COLS], int *count) {
    if (r < 0 || r >= GRID_ROWS || c < 0 || c >= GRID_COLS) return;
    if (!grid->bubbles[r][c].active || grid->bubbles[r][c].color != color || visited[r][c]) return;
    visited[r][c] = 1;
    (*count)++;
    // Altıgen gridde 6 komşu
    int dr[6] = {-1, -1, 0, 0, 1, 1};
    int dc_even[6] = {-1, 0, -1, 1, -1, 0}; // çift satır
    int dc_odd[6]  = {0, 1, -1, 1, 0, 1};   // tek satır
    for (int d = 0; d < 6; d++) {
        int nr = r + dr[d];
        int nc = c + ((r % 2 == 0) ? dc_even[d] : dc_odd[d]);
        dfs(grid, nr, nc, color, visited, count);
    }
}

int PopConnectedBubbles(BubbleGrid *grid, int row, int col) {
    int visited[GRID_ROWS][GRID_COLS] = {0};
    int count = 0;
    BubbleColor color = grid->bubbles[row][col].color;
    dfs(grid, row, col, color, visited, &count);
    if (count >= 3) {
        for (int r = 0; r < GRID_ROWS; r++)
            for (int c = 0; c < GRID_COLS; c++)
                if (visited[r][c]) grid->bubbles[r][c].active = 0;
        return count;
    }
    return 0;
}

// Tavana bağlı olmayan balonları BFS ile bulup düşürür
void DropFloatingBubbles(BubbleGrid *grid, int *score) {
    int visited[GRID_ROWS][GRID_COLS] = {0};
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
                int dr[6] = {-1, -1, 0, 0, 1, 1};
                int dc_even[6] = {-1, 0, -1, 1, -1, 0}; // çift satır
                int dc_odd[6]  = {0, 1, -1, 1, 0, 1};   // tek satır
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
    if (score) *score += dropped * 20;
} 