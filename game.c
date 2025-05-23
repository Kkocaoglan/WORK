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

// Oyun güncellenir (oyuncu, skor, oyun bitişi)
void UpdateGame(Game* game, GameState* state) {
    if (game->isGameOver) {
        *state = GAME_OVER;
        return;
    }

    // Check if all balls are cleared
    int totalBalls = 0;
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (game->grid.bubbles[r][c].active) {
                totalBalls++;
            }
        }
    }
    
    // If all balls are cleared, player wins
    if (totalBalls == 0) {
        game->isGameOver = 1;
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
                // Tüm toplar bitti mi kontrolü (patlama sonrası)
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
                NextBubble(&game->player, &game->grid);
            }
            else {
                // Patlama yoksa balon aktif kalmalı, yeni top gelmemeli
                game->player.shooting = 0;
                // Yine de tüm toplar bitti mi kontrolü (ör: son balon yerleştirildi ama patlama olmadı)
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
            }
        }
    }
    if (IsGridFull(&game->grid)) {
        game->isGameOver = 1;
    }
}

// Oyun ekranı çizilir
void DrawGame(const Game* game) {
    DrawBubbleGrid(&game->grid);
    DrawPlayer(&game->player, &game->grid);
    DrawScoreUI(game->score);
}