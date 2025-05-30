#define _CRT_SECURE_NO_WARNINGS
#include "game.h"
#include "raylib.h"
#include "collision.h"
#include "bubble.h"
#include "player.h"
#include "ui.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Ekran boyutları
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Renk tanımlamaları
#define LIGHT_PINK CLITERAL(Color){ 255, 192, 203, 255 }
#define WHITE CLITERAL(Color){ 255, 255, 255, 255 }
#define MAX_MOVES 80

// Oyun sonu kontrolü
static bool IsGameOver(const BubbleGrid* grid) {
    // En alt satıra ulaşıldı mı kontrol et
    for (int x = 0; x < GRID_COLS; x++) {
        if (grid->bubbles[GRID_ROWS - 1][x].active) {
            return true;
        }
    }
    return false;
}

// Oyunu başlat
void InitGame(Game* game) {
    game->score = 0;
    game->moves = 0;
    game->gameOver = false;
    game->paused = false;
    InitBubbleGrid(&game->grid);
    InitPlayer(&game->player);
}

// Oyunu güncelle
void UpdateGame(Game* game) {
    if (!game->paused && !game->gameOver) {
        UpdatePlayer(&game->player, &game->grid);
        UpdateParticles();  // Parçacıkları güncelle

        // Fırlatılan balonun çarpışma kontrolü
        if (game->player.shooting) {
            if (CheckBubbleCollision(&game->grid, &game->player.bubble)) {
                int row, col;
                PlaceBubble(&game->player.bubble, &game->grid, &row, &col);
                int popped = PopConnectedBubbles(&game->grid, row, col);
                if (popped > 0) {
                    game->score += popped * 10;  // Her patlatılan top için 10 puan ekle
                    printf("Patlatılan top sayısı: %d, Yeni skor: %d\n", popped, game->score);
                }
                NextBubble(&game->player, &game->grid);
                game->moves++;
                ResetExplosionState();
            }
        }

        // Oyun sonu kontrolü
        if (IsGameOver(&game->grid) || game->moves >= MAX_MOVES) {
            game->gameOver = true;
        }
        else {
            // Tüm toplar patlatıldı mı kontrol et
            bool hasBubbles = false;
            for (int r = 0; r < GRID_ROWS; r++) {
                for (int c = 0; c < GRID_COLS; c++) {
                    if (game->grid.bubbles[r][c].active) {
                        hasBubbles = true;
                        break;
                    }
                }
                if (hasBubbles) break;
            }

            if (!hasBubbles) {
                int remainingBubbles = MAX_MOVES - game->moves;
                game->score += remainingBubbles * 50;  // Kalan balonlar için 50 puan ekle
                game->gameOver = true;
            }
        }
    }
}

// Oyunu çiz
void DrawGame(Game* game) {
    // Arka plan
    ClearBackground(LIGHT_PINK);

    // Baloncuk ızgarasını çiz
    DrawBubbleGrid(&game->grid);

    // Parçacıkları çiz
    DrawParticles();

    // Oyuncuyu çiz
    DrawPlayer(&game->player, &game->grid);

    // Skor gösterimi
    char scoreText[32];
    sprintf(scoreText, "Score: %d", game->score);
    int textWidth = MeasureText(scoreText, 20);
    DrawText(scoreText, GetScreenWidth() - textWidth - 10, GetScreenHeight() - 30, 20, WHITE);

    // Atılan topun içine kalan top sayısını yaz
    if (game->player.shooting) {
        char moveText[8];
        int remainingBubbles = MAX_MOVES - (game->moves + 1);
        sprintf(moveText, "%d", remainingBubbles);
        int textWidth = MeasureText(moveText, 20);
        DrawText(moveText,
            game->player.bubble.pos.x - textWidth / 2,
            game->player.bubble.pos.y - 10,
            20, WHITE);
    }

    DrawUI(game);

    // Oyun sonu ekranını çiz
    if (game->gameOver) {
        DrawGameOverUI(game->score);
    }
}

// Oyunu temizle
void UnloadGame(Game* game) {
    UnloadBubbleGrid(&game->grid);
    UnloadPlayer(&game->player);
}