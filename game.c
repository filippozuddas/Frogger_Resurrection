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

    if(pipe(game->pipeFd) < 0 || pipe(game->mainToFrogPipe) < 0 || pipe(game->mainToCrocPipe) < 0) {
        perror("pipe creation error"); 
        exit(1); 
    }

    //adesso funziona anche senza pipe non bloccante (non so perchè)
    // int flags1 = fcntl(game->pipeFd[0], F_GETFL, 0);
    // fcntl(game->pipeFd[0], F_SETFL, flags1 | O_NONBLOCK);

    // int flags2 = fcntl(game->mainToFrogPipe[0], F_GETFL, 0);
    // fcntl(game->mainToFrogPipe[0], F_SETFL, flags2 | O_NONBLOCK);

    int flags3 = fcntl(game->mainToCrocPipe[0], F_GETFL, 0);
    fcntl(game->mainToCrocPipe[0], F_SETFL, flags3 | O_NONBLOCK);


    inizializza_mappa();
    disegna_mappa();
}


void runGame(Game* game) {
 
    createCroc(game->crocodile, game->pipeFd, game->mainToCrocPipe);
    createFrog(&(game->frog), game->pipeFd, game->mainToFrogPipe);

    Frog *tempFrog = &game->frog;
    Crocodile *croc = game->crocodile;
    Informations info;
    Grenade grenade;


    close(game->pipeFd[1]);
    close(game->mainToFrogPipe[0]);

    int playerCrocIdx = 0;

    while (game->isRunning) {
        if(read(game->pipeFd[0], &info, sizeof(Informations)) > 0){
            if(info.ID == 0) {
                tempFrog->info = info;
            }
            else if(info.ID >= 1 && info.ID <= 16) {
                croc[info.ID - 1].info = info; 

                if (playerCrocIdx == info.ID && tempFrog->isOnCroc){
                    tempFrog->info.x = info.x + tempFrog->onCrocOffset;
                }
            }
            else if(info.ID > 16){
                grenade.info = info;
            }
        }
        

        playerCrocIdx = isFrogOnCroc(game);
        write(game->mainToFrogPipe[1], &tempFrog->info, sizeof(Informations));

        if(tempFrog->isOnCroc == 0 && isFrogOnRiver(game)) {
            if(tempFrog->lives == 0) {
                game->isRunning = 0; 
                break;
            }
            //sleep(1);
            //resetCroc(croc);
            //write(game->mainToCrocPipe[1], &croc->info, sizeof(Informations));
            tempFrog->lives--; 
            tempFrog->info.x = ((COLS - 1) / 2) - 4; 
            tempFrog->info.y = LINES - 4;
        }

        werase(stdscr); 

        disegna_mappa();

        mvprintw(1, 1, "Player's lives: %d", tempFrog->lives);
        mvprintw(1, 20, "LINES: %d, COLS: %d", LINES, COLS);
        mvprintw(1, 50, "x = %d, y = %d", tempFrog->info.x, tempFrog->info.y);

        for (int i = 0; i < N_CROC; i++) {
            printCroc(croc[i].info.x, croc[i].info.y, croc[i].info.direction);
        }

        printFrog(tempFrog->info.x, tempFrog->info.y);

        mvprintw(grenade.info.y, grenade.info.x, "*");
        
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