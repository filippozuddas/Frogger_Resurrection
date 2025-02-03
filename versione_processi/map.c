#include "map.h"


void inizializza_mappa() {
    // Inizializzazione colori
    init_pair(1, COLOR_BLACK, COLOR_GREEN);  // Marciapiede e tane
    init_pair(2, COLOR_BLACK, COLOR_BLUE);   // Fiume
    init_pair(3, COLOR_BLACK, COLOR_MAGENTA); // Sponda argine
}

void disegna_blocco(WINDOW *win, int starty, int startx, int height, int width, int color_pair) {
    wattron(win, COLOR_PAIR(color_pair));
    for (int y = starty; y < starty + height; y++) {
        for (int x = startx; x < startx + width; x++) {
            mvwaddch(win, y, x, ' '); // Blocchi vuoti con colore di sfondo
        }
    }
    wattroff(win, COLOR_PAIR(color_pair));
}

void disegna_mappa(Game *game) {
    // Disegna il marciapiede di partenza
    disegna_blocco(game->gameWin, GAME_HEIGHT - 5, 0, 4, GAME_WIDTH, 1);

    // Disegna l'area del fiume
    disegna_blocco(game->gameWin, 4, 0, GAME_HEIGHT - 9, GAME_WIDTH, 2);

    // Disegna la sponda superiore (argine)
    disegna_blocco(game->gameWin, 11, 0, 4, GAME_WIDTH, 3);

    // Disegna 5 tane equidistanti
    for (int i = 0; i < N_DENS; i++) {
        Den *den = &game->dens[i]; 
        if (den->isOpen) {
            disegna_blocco(game->gameWin, den->y, den->x, den->height, den->width, 1); 
        }
        else {
            disegna_blocco(game->gameWin, den->y, den->x, den->height, den->width, 1);
            printFrog(game->gameWin, den->x, den->y); 
        }
    }

    //box(game->gameWin, 0, 0); 
}

