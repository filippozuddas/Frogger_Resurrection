#include "game.h"

int main() {
    Game game; 
    game.isRunning = 0; 

    initGame(&game); 
    runGame(&game); 
    stopGame(&game); 
}