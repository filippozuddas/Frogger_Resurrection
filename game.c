#include "game.h"
#include <signal.h>
#include <locale.h>
#include <fcntl.h>


void initGame(Game *game) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
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
    MessageType type;

    createCroc(game->crocodile, game->pipeFd);
    createFrog(&(game->frog), game->pipeFd); 

    close(game->pipeFd[1]);

    Crocodile croc[N_CROC]; 
    memset(croc, 0, sizeof(croc)); 

    while (game->isRunning) {
        int ch = getch();
        
        // Esci con 'q' o 'Q'
        if (ch == 'q' || ch == 'Q') {
            break;
        }

        if (read(game->pipeFd[0], &type, sizeof(type)) > 0) {
            if(type == MSG_CROC){
                if (read(game->pipeFd[0], &tempCroc, sizeof(Crocodile)) > 0) {
                    // Trova l'indice corrispondente al pid o a un posto vuoto
                    int found = -1;
                    for (int i = 0; i < N_CROC; i++) {
                        if (croc[i].pid == tempCroc.pid || croc[i].pid == 0) {
                            found = i;
                            break;
                        }
                    }

                    if (found != -1) {
                        croc[found] = tempCroc;
                    }

                    // non so perchè funziona meglio di clear()
                    /*
                    werase(stdscr);
                    
                    for (int i = 0; i < N_CROC; i++) {
                        printCroc(croc[i].x, croc[i].y, croc[i].direction); 
                    }

                    refresh();
                    */
                } 
            }
            else if (type == MSG_FROG)
            {
               
                if(read(game->pipeFd[0], &tempFrog, sizeof(Frog)) > 0);
                    game->frog = tempFrog;
                //werase(stdscr); 
                /*printFrog(tempFrog.x, tempFrog.y); 
                
                for (int i = 0; i < N_CROC; i++) {
                    printCroc(croc[i].x, croc[i].y, croc[i].direction);
                }
                
                refresh(); 
                */
                
            }
        }
        werase(stdscr); 

        for (int i = 0; i < N_CROC; i++) {
            printCroc(croc[i].x, croc[i].y, croc[i].direction);
        }

        printFrog(game->frog.x, game->frog.y);

        refresh();
    }
}

void stopGame(Game *game) {
    //Termina tutti i processi figli
    for (int i = 0; i < N_CROC; i++) {
        kill(game->crocodile[i].pid, SIGTERM);
    }

    // Aspetta che tutti i processi figli terminino
    for (int i = 0; i < N_CROC; i++) {
        waitpid(game->crocodile[i].pid, NULL, 0);
    }
    
    endwin(); 
}