#include "game.h"
#include <locale.h>

int main() {
    //attiva i caratteri speciali dell'UNICODE
    setlocale(LC_ALL, ""); 

    Game game; 
    game.isRunning = 0; 

    initGame(&game); 
    runGame(&game); 
    stopGame(&game); 
}