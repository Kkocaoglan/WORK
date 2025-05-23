#include "game.h"
#include <stdlib.h>

// Oyun başlatılır
void InitGame(Game* game) {
    InitBubbleGrid(&game->grid);
    InitPlayer(&game->player);
    game->score = 0;
    game->level = 1;
    game->isGameOver = 0;
}

// Oyun sıfırlanır
void ResetGame(Game* game) {
    InitGame(game);
}

// Kalan renkleri bul
static void FindRemainingColors(const BubbleGrid* grid, int* colorCount, BubbleColor* remainingColors) {
    int colorExists[BUBBLE_COLORS] = {0};  // Her rengin var olup olmadığını tut
    *colorCount = 0;

    // Griddeki tüm aktif balonları kontrol et
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid->bubbles[r][c].active) {
                BubbleColor color = grid->bubbles[r][c].color;
                if (!colorExists[color]) {
                    colorExists[color] = 1;
                    remainingColors[(*colorCount)++] = color;
                }
            }
        }
    }
}

// Kalan renklerden rastgele bir balon oluştur
static Bubble CreateBubbleFromRemainingColors(const BubbleGrid* grid) {
    int colorCount;
    BubbleColor remainingColors[BUBBLE_COLORS];
    FindRemainingColors(grid, &colorCount, remainingColors);

    Bubble bubble;
    bubble.active = 1;
    bubble.pos = (Vector2){0, 0};  // Başlangıç pozisyonu

    // Eğer 2 veya daha az renk kaldıysa, sadece o renklerden birini seç
    if (colorCount <= 2) {
        bubble.color = remainingColors[GetRandomValue(0, colorCount - 1)];
    } else {
        // Normal durumda rastgele renk seç
        bubble.color = GetRandomValue(0, BUBBLE_COLORS - 1);
    }

    return bubble;
}

// Oyun güncellenir (oyuncu, skor, oyun bitişi)
void UpdateGame(Game* game, GameState* state) {
    if (game->isGameOver) {
        *state = GAME_OVER;
        return;
    }

    UpdatePlayer(&game->player, &game->grid);
    
    // Fırlatılan top çarptıysa işlemleri yap
    if (game->player.shooting) {
        int row, col;
        if (CheckBubbleCollision(&game->player.bubble, &game->grid)) {
            PlaceBubble(&game->player.bubble, &game->grid, &row, &col);
            int popped = PopConnectedBubbles(&game->grid, row, col);
            if (popped >= 3) {
                game->score += popped * 50;
                int dropped = DropFloatingBubbles(&game->grid);
                if (dropped > 0) game->score += dropped * 100;
                
                // Tüm toplar bitti mi kontrolü
                int totalBalls = 0;
                for (int r = 0; r < GRID_ROWS; r++) {
                    for (int c = 0; c < GRID_COLS; c++) {
                        if (game->grid.bubbles[r][c].active) totalBalls++;
                    }
                }
                if (totalBalls == 0) {
                    game->isGameOver = 1;
                    *state = GAME_OVER;
                    return;
                }
                // Yeni top oluştur (kalan renklerden)
                game->player.bubble = CreateBubbleFromRemainingColors(&game->grid);
                game->player.shooting = 0;
            }
            else {
                // Patlama yoksa balon aktif kalmalı
                game->player.shooting = 0;
                // Yeni top oluştur (kalan renklerden)
                game->player.bubble = CreateBubbleFromRemainingColors(&game->grid);
            }
        }
    }

    if (IsGridFull(&game->grid)) {
        game->isGameOver = 1;
        *state = GAME_OVER;
    }
}

// Oyun ekranı çizilir
void DrawGame(const Game* game) {
    DrawBubbleGrid(&game->grid);
    DrawPlayer(&game->player, &game->grid);
    DrawScoreUI(game->score);
}