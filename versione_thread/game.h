#pragma once

#include "librerie.h"


extern ScoreNode* scoreList;


void initDens(Game *game);
void initGame(Game *game);
void runGame(Game* game, int game_socket_fd);
void stopGame(Game *game);

void printGameOver(WINDOW *win);
void printYouWon(WINDOW *win);

void drawLives(WINDOW *win, int lives);
void drawTimer(Game *game, WINDOW *win, int timeLeft, int timerMax);
int timerHandler(Game *game, int *millisecondCounter, int countdownTime, int timerMax);


