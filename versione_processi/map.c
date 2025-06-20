#include "map.h"

// Inizializza la mappa del gioco
void inizializza_mappa() {
    // Inizializzazione colori
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);  // Marciapiede e tane
    init_pair(2, COLOR_BLACK, COLOR_BLUE);   // Fiume
    init_pair(3, COLOR_BLACK, COLOR_GREEN); // Sponda argine
}

// Stampa un blocco di colore specifico nella finestra di gioco
void disegna_blocco(WINDOW *win, int starty, int startx, int height, int width, int color_pair) {
    wattron(win, COLOR_PAIR(color_pair));
    for (int y = starty; y < starty + height; y++) {
        for (int x = startx; x < startx + width; x++) {
            mvwaddch(win, y, x, ' '); // Blocchi vuoti con colore di sfondo
        }
    }
    wattroff(win, COLOR_PAIR(color_pair));
}

// Stampa la mappa completa, comprese le tane
void disegna_mappa(Game *game) {
    // Disegna il marciapiede di partenza
    disegna_blocco(game->gameWin, 5, 0, 60, GAME_WIDTH, 2);  // Fiume
    disegna_blocco(game->gameWin, 9, 0, 4, GAME_WIDTH, 3);    // Argine superiore
    disegna_blocco(game->gameWin, 65, 0, 4, GAME_WIDTH, 1);  // Argine inferiore   
    // Disegna 5 tane equidistanti
    for (int i = 0; i < N_DENS; i++) {
        Den *den = &game->dens[i]; 
        if (den->isOpen) {
            printDen(game, den->x, den->y);
        }
        else {
            printDen(game, den->x, den->y);
            printFrogOnDen(game->gameWin, den->x, den->y); 
        }
    }
}


