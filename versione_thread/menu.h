#pragma once 

#include "librerie.h"
#include "game.h"
#include <ncursesw/ncurses.h>


void showMenu(Game *game);
void animate_welcome();
void init_window_position();
void mainMenu(Game *game);
void levelMenu(Game *game);
void print_menu(WINDOW *menu_win, int highlight, MenuOption menu[], int n_choices);
void displayScoreMenu(Game *game);
void startGame(Game * game);

