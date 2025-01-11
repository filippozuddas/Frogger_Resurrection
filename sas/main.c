#include "map.h"
#include <ncurses.h>

int main() {
    inizializza_mappa();   // Inizializza ncurses
    disegna_mappa();       // Disegna la mappa

    getch();               // Attendi input per terminare
    chiudi_mappa();        // Chiudi ncurses
    return 0;
}