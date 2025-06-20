#pragma once

#include <ncurses.h>
#include "struct.h"
#include "sprite.h"

void inizializza_mappa();
void disegna_blocco(WINDOW *win, int starty, int startx, int altezza, int larghezza, int color_pair);
void disegna_mappa(Game *game);