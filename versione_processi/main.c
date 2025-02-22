#include "game.h"
#include "menu.h"
#include <locale.h>

int main() {
    //attiva i caratteri speciali dell'UNICODE
    setlocale(LC_ALL, ""); 
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    
    Game game; 
    showMenu(&game); 
    endwin();
}