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

    if(pipe(game->pipeFd) < 0) {
        perror("pipe creation error"); 
        exit(1); 
    }

    int flags = fcntl(game->pipeFd[0], F_GETFL, 0);
    fcntl(game->pipeFd[0], F_SETFL, flags | O_NONBLOCK);

    inizializza_mappa();
    disegna_mappa();
}


void runGame(Game* game) {


    Crocodile tempCroc;
    Frog tempFrog; 
    Informations info;

    createCroc(game->crocodile, game->pipeFd);
    createFrog(game->frog, game->pipeFd);
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

        // Esempio di collision check e spostamento nel padre
        // (Da integrare in runGame, dopo aver letto i dati da pipe)
        int frogOnIndex = -1; // -1 se non su coccodrillo
        for (int i = 0; i < N_CROC; i++) {
            if (checkCollision(tempFrog.info, croc[i].info)) {
                frogOnIndex = i;
                break;
            }
        }

        // Se hai trovato un coccodrillo compatibile
        if (frogOnIndex != -1) {
            tempFrog.isOnCroc = 1;
            tempFrog.onCrocIdx = frogOnIndex;
        } else {
            tempFrog.isOnCroc = 0;
        }

        // Se la rana risulta su un coccodrillo, spostala insieme ad esso
        if (tempFrog.isOnCroc) {
            int idx = tempFrog.onCrocIdx;
            // Logica di “frog che si muove col coccodrillo”
            if (croc[idx].info.direction == 0) {
                tempFrog.info.x += croc[idx].info.speed;
            } else {
                tempFrog.info.x -= croc[idx].info.speed;
            }
            // Controlla se cade
            if (tempFrog.info.x < 0 || tempFrog.info.x >= COLS) {
                tempFrog.isOnCroc = 0; 
                // Esempio: togli vita
                tempFrog.lives -= 1;
                // Riportala al punto iniziale
                tempFrog.info.x = (COLS - 1)/2; 
                tempFrog.info.y = LINES - 4;
            }
        }

        werase(stdscr); 
        //clear();

        disegna_mappa();

        for (int i = 0; i < N_CROC; i++) {
            printCroc(croc[i].info.x, croc[i].info.y, croc[i].info.direction);
        }

        printFrog(tempFrog.info.x, tempFrog.info.y);
        
        refresh();

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