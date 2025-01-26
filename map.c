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

void disegna_mappa(WINDOW *win) {
    // Disegna il marciapiede di partenza
    disegna_blocco(win, GAME_HEIGHT - 5, 0, 4, GAME_WIDTH, 1);

    // Disegna l'area del fiume
    disegna_blocco(win, 4, 0, GAME_HEIGHT - 9, GAME_WIDTH, 2);

    // Disegna la sponda superiore (argine)
    disegna_blocco(win, 11, 0, 4, GAME_WIDTH, 3);

    // Disegna 5 tane equidistanti
    int larghezzaTana = 10;
    int spaziaturaTane = (GAME_WIDTH - 5 * larghezzaTana) / 6; // Spazio tra tane
    for (int i = 0; i < 5; i++) {
        int startx = spaziaturaTane + i * (larghezzaTana + spaziaturaTane);
        disegna_blocco(win, 7, startx, 4, larghezzaTana, 1);
    }

    //box(win, 0, 0); 
}

