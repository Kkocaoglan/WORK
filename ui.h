#ifndef UI_H
#define UI_H

// Menü arayüzü
void DrawMenuUI(void);
int UpdateMenuUI(void);

// Skor ve level gösterimi
void DrawScoreUI(int score);
void DrawLevelUI(int level, int targetScore);

// Oyun duraklatma arayüzü
void DrawPauseUI(void);

// Level tamamlama arayüzü
void DrawLevelCompleteUI(int level, int score);

// Oyun sonu arayüzü
void DrawGameOverUI(int score);
int UpdateGameOverUI(void);

#endif // UI_H 