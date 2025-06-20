#pragma once 

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>

#include <ncursesw/curses.h>  
#include <menu.h>            

#include "struct.h"  
#include "game.h"
#include "sprite.h"

#define MAX_HEIGHT_WELCOME 26
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void animate_welcome();
void init_window_position();
int mainMenu(Game *game);
int levelMenu(Game *game);
void print_menu(WINDOW *menu_win, int highlight, MenuOption menu[], int n_choices);
void displayScoreMenu(Game *game);

