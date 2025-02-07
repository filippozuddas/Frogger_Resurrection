#pragma once

#include <curses.h>
#include "crocodile.h"
#include "frog.h"
#include "colors.h"

void printCroc(WINDOW *win, int x, int y, int direction); 
void printFrog(Game *game, WINDOW *win, int x, int y); 
void printFrogOnDen(WINDOW *win, int x, int y);