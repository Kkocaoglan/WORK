#include "game.h"
#include <stdlib.h>

// Oyun başlatılır
void InitGame(Game *game) {
    InitBubbleGrid(&game->grid);
    InitPlayer(&game->player);
    game->score = 0;
    game->level = 1;
    game->isGameOver = 0;
}

// Oyun sıfırlanır
void ResetGame(Game *game) {
    InitGame(game);
}

// Oyun güncellenir (oyuncu, skor, oyun bitişi)
void UpdateGame(Game *game, GameState *state) {
    if (game->isGameOver) {
        *state = GAME_OVER;
        return;
    }
    UpdatePlayer(&game->player, &game->grid);
    // TODO: Balon çarpışma ve patlatma, skor, seviye ilerleme
    if (IsGridFull(&game->grid)) {
        game->isGameOver = 1;
    }
}

// Oyun ekranı çizilir
void DrawGame(const Game *game) {
    DrawBubbleGrid(&game->grid);
    DrawPlayer(&game->player);
    // TODO: Skor ve seviye gösterimi
} 