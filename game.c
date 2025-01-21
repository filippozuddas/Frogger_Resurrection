#include "game.h"
#include <signal.h>
#include <fcntl.h>


void initGame(Game *game) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();

    setColors(); 
    srand(time(NULL)); 

    game->isRunning = 1; 

    if(pipe(game->pipeFd) < 0 || pipe(game->mainToEntPipe) < 0) {
        perror("pipe creation error"); 
        exit(1); 
    }

    //adesso funziona anche senza pipe non bloccante (non so perchè)
    int flags1 = fcntl(game->pipeFd[0], F_GETFL, 0);
    fcntl(game->pipeFd[0], F_SETFL, flags1 | O_NONBLOCK);

    int flags2 = fcntl(game->mainToEntPipe[0], F_GETFL, 0);
    fcntl(game->mainToEntPipe[0], F_SETFL, flags2 | O_NONBLOCK);

    inizializza_mappa();
    disegna_mappa();
}


void runGame(Game* game) {
 
    createCroc(game->crocodile, game->pipeFd);
    createFrog(&(game->frog), game->pipeFd, game->mainToEntPipe);

    Frog *tempFrog = &game->frog;
    Crocodile *croc = game->crocodile;
    Informations info;

    close(game->pipeFd[1]);
    close(game->mainToEntPipe[0]);

    int playerCrocIdx = 0;

    while (game->isRunning) {
        if(read(game->pipeFd[0], &info, sizeof(Informations)) > 0){
            if(info.ID == 0) {
                tempFrog->info = info;
            }
            if(info.ID >= 1) {
                croc[info.ID - 1].info = info; 

                if (playerCrocIdx == info.ID && tempFrog->isOnCroc){
                    tempFrog->info.x = info.x + tempFrog->onCrocOffset;
                }
            }
        }
        

        playerCrocIdx = isFrogOnCroc(game);
        if (playerCrocIdx > 0) {
            tempFrog->isOnCroc = 1; 
        }
        else {
            tempFrog->isOnCroc = 0; 
        }
        write(game->mainToEntPipe[1], &game->frog.info, sizeof(Informations));


        werase(stdscr); 

        disegna_mappa();

        mvprintw(1, 1, "Player's lives: %d", tempFrog->lives);
        mvprintw(1, 20, "LINES: %d, COLS: %d", LINES, COLS);
        mvprintw(1, 50, "x = %d, y = %d", tempFrog->info.x, tempFrog->info.y);

        for (int i = 0; i < N_CROC; i++) {
            printCroc(croc[i].info.x, croc[i].info.y, croc[i].info.direction);
        }

        printFrog(tempFrog->info.x, tempFrog->info.y);
        
        refresh();

        usleep(1000);
    }
}


void stopGame(Game *game) {
    //Termina tutti i processi figli
    for (int i = 0; i < N_CROC; i++) {
        kill(game->crocodile[i].info.pid, SIGTERM);
    }

    // Aspetta che tutti i processi figli terminino
    for (int i = 0; i < N_CROC; i++) {
        waitpid(game->crocodile[i].info.pid, NULL, 0);
    }
    
    endwin(); 
}