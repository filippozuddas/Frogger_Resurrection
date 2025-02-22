#pragma once

#include "colors.h"
#include "crocodile.h"
#include "frog.h"
#include "map.h"
#include "sprite.h"
#include "colors.h"
#include "struct.h"
#include "map.h"
#include "communication.h"

extern ScoreNode* scoreList;


void initDens(Game *game);
void initGame(Game *game); 
void runGame(Game *game); 
void stopGame(Game *game); 
void flushPipe(int fd);
void setNonBlocking(int fd);


int timerHandler(Game *game, int *millisecondCounter, int countdownTime, int timerMax);
void drawTimer(Game *game, WINDOW *win, int timeLeft, int timeMax) ;
void drawLives(WINDOW *win, int lives);

void restartMenu(Game *game, int score, int flag);
void printGameOver(WINDOW *win);
void printYouWon(WINDOW *win);

ScoreNode* createNode(int score);
void addScore(ScoreNode** head, int score);
void handleScores(Game *game, int countdownTime, int isDead);
void saveScores(ScoreNode* head);
void loadScores(ScoreNode** head);
void displayLargeScore(Game *game);
void printLargeScore(WINDOW *win, int score, int startY, int startX);
void printScoreSprite(WINDOW *win, int startY, int startX);
void printDigit(WINDOW *win, int digit, int startX, int startY);
void digitsAnalyser(WINDOW *win, int score, int starty, int startx);
int digitsCount(int numero);



