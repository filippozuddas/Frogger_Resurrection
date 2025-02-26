#include "struct.h"
#include <ncursesw/ncurses.h>


void handleScores(Game *game, int countdownTime, int isDead);
ScoreNode* createNode(int score);
void addScore(ScoreNode** head, int score);
void saveScores(ScoreNode* head);
void loadScores(ScoreNode** head);
void printDigit(WINDOW *win, int digit, int startX, int startY);
int digitsCount(int numero);
void digitsAnalyser(WINDOW *win, int score, int starty, int startx);