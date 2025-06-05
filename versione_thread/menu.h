#pragma once 

#include "librerie.h"


void animate_welcome();
void init_window_position();
int mainMenu(Game *game);
int levelMenu(Game *game);
void print_menu(WINDOW *menu_win, int highlight, MenuOption menu[], int n_choices);
void displayScoreMenu(Game *game);
void startGame(Game * game);

