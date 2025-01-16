#include "game.h"
#include <signal.h>
#include <locale.h>
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

    setlocale(LC_ALL, ""); 

    game->isRunning = 1; 

    if(pipe(game->pipeFd) < 0) {
        perror("pipe creation error"); 
        exit(1); 
    }

    int flags = fcntl(game->pipeFd[0], F_GETFL, 0);
    fcntl(game->pipeFd[0], F_SETFL, flags | O_NONBLOCK);
}


void runGame(Game* game) {


    Crocodile tempCroc;
    Frog tempFrog; 
    //MessageType type;
    Informations info;

    createCroc(game->crocodile, game->pipeFd);
    createFrog(game->frog, game->pipeFd, game);
    tempFrog = game->frog;

    close(game->pipeFd[1]);

    Crocodile croc[N_CROC]; 
    memset(croc, 0, sizeof(croc)); 

    while (game->isRunning) {
        /*int ch = getch();
        
        // Esci con 'q' o 'Q'
        if (ch == 'q' || ch == 'Q') {
            break;
        }*/

        while (read(game->pipeFd[0], &info, sizeof(Informations)) > 0){
            if(info.ID == 1) {
                int found = -1;
                for (int i = 0; i < N_CROC; i++) {
                    if (croc[i].info.pid == info.pid || croc[i].info.pid == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    croc[found].info = info;
                }
            }
            if(info.ID == 0) {
                tempFrog.info = info;
            }
        }

        werase(stdscr); 
        //clear();

        for (int i = 0; i < N_CROC; i++) {
            printCroc(croc[i].info.x, croc[i].info.y, croc[i].info.direction);
        }

        printFrog(tempFrog.info.x, tempFrog.info.y);
        
        wrefresh(stdscr);

        usleep(16000);
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