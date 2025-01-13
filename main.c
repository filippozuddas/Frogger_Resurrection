#include "game.h"
#include <locale.h>

int main() {
    //setlocale(LC_ALL, ""); 

    Game game; 
    game.isRunning = 0; 

    initGame(&game); 
    runGame(&game); 
    stopGame(&game); 
}