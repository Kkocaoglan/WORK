#define _CRT_SECURE_NO_WARNINGS
#include "game.h"
#include "raylib.h"
#include "collision.h"
#include "bubble.h"
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

// Bir topun yanında aynı renkte başka top var mı kontrol et
static bool HasMatchingNeighbor(const BubbleGrid* grid, int row, int col) {
    BubbleColor color = grid->bubbles[row][col].color;

    // Komşu pozisyonları kontrol et
    int neighbors[6][2] = {
        {row - 1, col},    // üst
        {row - 1, col + 1},  // üst sağ
        {row, col + 1},    // sağ
        {row + 1, col},    // alt
        {row + 1, col - 1},  // alt sol
        {row, col - 1}     // sol
    };

    for (int i = 0; i < 6; i++) {
        int r = neighbors[i][0];
        int c = neighbors[i][1];

        // Geçerli pozisyon mu kontrol et
        if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
            // Aynı renkte ve aktif bir top var mı?
            if (grid->bubbles[r][c].active && grid->bubbles[r][c].color == color) {
                return true;
            }
        }
    }

    return false;
}

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
void UpdateGame(Game* game, GameState* gameState) {
    if (game->paused) {
        *gameState = GAME_STATE_PAUSED;
        return;
    }

    if (game->gameOver) {
        *gameState = GAME_STATE_GAME_OVER;
        return;
    }

    // Oyuncuyu güncelle
    game->player.moves = game->moves;  // Hamle sayısını oyuncuya aktar
    UpdatePlayer(&game->player, &game->grid);

    // Çarpışma kontrolü
    if (game->player.shooting) {
        if (CheckBubbleCollision(&game->grid, &game->player.bubble)) {
            // Hamle sayısını artır
            game->moves++;

            // Baloncuk yerleştirildi
            int row, col;
            PlaceBubble(&game->player.bubble, &game->grid, &row, &col);

            // Patlama kontrolü
            int popped = PopConnectedBubbles(&game->grid, row, col);
            if (popped > 0) {
                game->score += popped * 10;
            }

            // Havada kalan balonları düşür
            int dropped = DropFloatingBubbles(&game->grid);
            if (dropped > 0) {
                game->score += dropped * 5;
            }

            // Yeni baloncuk hazırla
            game->player.shooting = 0;
            NextBubble(&game->player, &game->grid);

            // Oyun sonu kontrolü
            if (IsGameOver(&game->grid)) {
                game->gameOver = true;
            }
            // Hamle sınırı kontrolü
            else if (game->moves >= MAX_MOVES) {
                game->gameOver = true;
            }

            // Ekranda top kalmadıysa ve hamle sayısı bitmediyse bonus puan ekle
            if (!IsGameOver(&game->grid) && game->moves < MAX_MOVES) {
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
                    game->score += remainingBubbles * 50;
                    game->gameOver = true;
                }
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

    // Oyuncuyu çiz
    DrawPlayer(&game->player, &game->grid);

    // Skor gösterimi
    char scoreText[32];
    sprintf(scoreText, "Skor: %d", game->score);
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
}

// Oyunu temizle
void UnloadGame(Game* game) {
    // Oyun kaynaklarını temizle
    UnloadBubbleGrid(&game->grid);
}