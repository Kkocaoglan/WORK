#ifndef UI_H
#define UI_H

// Menü arayüzü
void DrawMenuUI(void);
int UpdateMenuUI(void);

// Skor gösterimi
void DrawScoreUI(int score);

// Oyun duraklatma arayüzü
void DrawPauseUI(void);

// Oyun sonu arayüzü
void DrawGameOverUI(int score);
int UpdateGameOverUI(void);

#endif // UI_H 