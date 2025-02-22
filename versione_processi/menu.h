#pragma once 

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>

// Poi le librerie ncurses nell'ordine corretto
#include <ncursesw/curses.h>  // Prima curses.h
#include <menu.h>             // Poi menu.h

// Infine i tuoi header personalizzati
#include "struct.h"  // Il tuo header con le strutture
#include "game.h"
#include "sprite.h"

#define MAX_HEIGHT_WELCOME 26
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))



void showMenu(Game *game);
void animate_welcome();
void init_window_position();
void mainMenu(Game *game);
void levelMenu(Game *game);
void print_menu(WINDOW *menu_win, int highlight, MenuOption menu[], int n_choices);
void displayScoreMenu(Game *game);

