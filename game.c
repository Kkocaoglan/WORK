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
                NextBubble(&game->player);
            }
            else {
                // Patlama yoksa balon aktif kalmalı, yeni top gelmemeli
                game->player.shooting = 0;
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
    DrawPlayer(&game->player);
    DrawScoreUI(game->score);
}