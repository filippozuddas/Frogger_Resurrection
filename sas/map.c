#include <ncurses.h>
#include "map.h"

// Dimensioni dello schermo
#define LARGHEZZA COLS
#define ALTEZZA LINES

void inizializza_mappa() {
    initscr();     
    noecho();             
    curs_set(0);      
    start_color();    

    // Inizializzazione colori
    init_pair(1, COLOR_BLACK, COLOR_GREEN);  // Marciapiede e tane
    init_pair(2, COLOR_BLACK, COLOR_BLUE);   // Fiume
    init_pair(3, COLOR_BLACK, COLOR_YELLOW); // Sponda argine
}

void disegna_blocco(int starty, int startx, int altezza, int larghezza, int color_pair) {
    attron(COLOR_PAIR(color_pair));
    for (int y = starty; y < starty + altezza; y++) {
        for (int x = startx; x < startx + larghezza; x++) {
            mvaddch(y, x, ' '); // Blocchi vuoti con colore di sfondo
        }
    }
    attroff(COLOR_PAIR(color_pair));
}

void disegna_mappa() {
    // Cancella lo schermo
    clear();

    // Disegna il marciapiede di partenza
    disegna_blocco(ALTEZZA - 3, 0, 3, LARGHEZZA, 1);

    // Disegna l'area del fiume
    disegna_blocco(3, 0, ALTEZZA - 6, LARGHEZZA, 2);

    // Disegna la sponda superiore (argine)
    disegna_blocco(4, 0, 6, LARGHEZZA, 3);

    // Disegna 5 tane equidistanti
    int larghezza_tana = 10;
    int spaziatura_tane = (LARGHEZZA - 5 * larghezza_tana) / 6; // Spazio tra tane
    for (int i = 0; i < 5; i++) {
        int startx = spaziatura_tane + i * (larghezza_tana + spaziatura_tane);
        disegna_blocco(4, startx, 3, larghezza_tana, 1);
    }

    refresh(); // Aggiorna la finestra
}

void chiudi_mappa() {
    endwin(); // Chiude ncurses
}
