#pragma once

#include <curses.h>
#include <wchar.h>
#include "crocodile.h"
#include "frog.h"
#include "struct.h"
#include "colors.h"

extern MenuOption menu[MENU_ITEMS];
extern MenuOption level[LEVEL_ITEMS];
extern MenuOption restart[RESTART_ITEMS];

void printCroc(WINDOW *win, int x, int y, int direction); 
void printFrog(Game *game, WINDOW *win, int x, int y); 
void printFrogOnDen(WINDOW *win, int x, int y);
void printGrenades(Game *game);
void printProjectiles(Game *game);
void printDen(Game *game, int x, int y);