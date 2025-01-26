#pragma once

#include <curses.h>
#include "crocodile.h"
#include "frog.h"
#include "colors.h"

void printCroc(WINDOW *win, int x, int y, int direction); 
void printFrog(WINDOW *win, int x, int y); 