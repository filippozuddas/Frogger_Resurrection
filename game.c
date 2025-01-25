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
    int flags1 = fcntl(game->pipeFd[0], F_GETFL, 0);
    fcntl(game->pipeFd[0], F_SETFL, flags1 | O_NONBLOCK);

    int flags2 = fcntl(game->mainToFrogPipe[0], F_GETFL, 0);
    fcntl(game->mainToFrogPipe[0], F_SETFL, flags2 | O_NONBLOCK);

    int flags3 = fcntl(game->mainToCrocPipe[0], F_GETFL, 0);
    fcntl(game->mainToCrocPipe[0], F_SETFL, flags3 | O_NONBLOCK);


    inizializza_mappa();
    disegna_mappa();
}


void runGame(Game* game) {

    /* Creazione e inizializzazione dei coccodrilli e della rana (player) */
    createCroc(game->crocodile, game->pipeFd, game->mainToCrocPipe);
    createFrog(game);

    
    /*
     * Inizializzazione dell'array di granate. 
     * ID = -1 indica che nessuna granata è associata a quell'elemento dell'array
     */
    for (int i = 0; i < MAX_GRENADES; i++) {
        game->grenades[i].info.ID = -1;  
    }

    /*
     * Per comodità creo dei puntatori per gestire la rana e i coccodrilli, 
     * senza dover scrivere ogni volta 'game->...'
     */

    Frog *tempFrog = &game->frog;
    Crocodile *croc = game->crocodile;
    Grenade *grenades = game->grenades;
    
    /*
     * Struttura Informations per memorizzare i dati (coordinate e altre info) letti dalla pipe, 
     * provenienti dalle varie entità di gioco dinamiche 
     */
    Informations info;

    close(game->pipeFd[1]);
    close(game->mainToFrogPipe[0]);

    /* Indice del del coccodrillo sul quele si trova la rana */
    int playerCrocIdx = 0;


    /* Ciclo principale di gestione del gioco */
    while (game->isRunning) {
        /* Lettura dalla pipe */
        while(read(game->pipeFd[0], &info, sizeof(Informations)) > 0){
            /* ID = 0 corrisponde alla rana */
            if(info.ID == 0) {
                tempFrog->info = info;
            }
            /* ID tra 1 e 16 corrispondono ai coccodrilli */
            else if(info.ID >= 1 && info.ID <= 16) {
                croc[info.ID - 1].info = info; 

                if (playerCrocIdx == info.ID && tempFrog->isOnCroc){
                    tempFrog->info.x = info.x + tempFrog->onCrocOffset;
                }
            }
            /* ID > 16 corrispondono alle granate della rana */
            else if(info.ID > 16){
                int found = 0;
                for (int i = 0; i < MAX_GRENADES; i++) {
                    if (grenades[i].info.ID == info.ID) {
                        grenades[i].info = info;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    // Aggiungi una nuova granata
                    for (int i = 0; i < MAX_GRENADES; i++) {
                        if (grenades[i].info.ID == -1) {
                            grenades[i].info = info;
                            break;
                        }
                    }
                }
            }
        }
        
        /* Verifico se la rana si trova sopra un coccodrillo */
        playerCrocIdx = isFrogOnCroc(game);

        /* 
         * Attraverso una pipe inversa (dal padre al processo rana) comunico le nuove coordinate
         * alla rana, nel caso in cui sia sopra un coccodrillo  
         */
        write(game->mainToFrogPipe[1], &tempFrog->info, sizeof(Informations));

        /* Verifico se la rana è caduta in acqua */
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

        /* Stampo le entità di gioco */
        werase(stdscr); 

        disegna_mappa();

        mvprintw(1, 1, "Player's lives: %d", tempFrog->lives);
        mvprintw(1, 20, "LINES: %d, COLS: %d", LINES, COLS);
        mvprintw(1, 50, "x = %d, y = %d", tempFrog->info.x, tempFrog->info.y);

        for (int i = 0; i < N_CROC; i++) {
            printCroc(croc[i].info.x, croc[i].info.y, croc[i].info.direction);
        }

        printFrog(tempFrog->info.x, tempFrog->info.y);
        
        for (int i = 0; i < MAX_GRENADES; i++) {
            if (grenades[i].info.ID != -1) {
                mvprintw(grenades[i].info.y, grenades[i].info.x, "*");
                /* 
                 * Se la granata è uscita dallo schermo, libero lo slot rendendolo disponibile
                 * per un altra granata 
                 */
                if (grenades[i].info.x < 0 || grenades[i].info.x >= COLS) {
                    grenades[i].info.ID = -1;
                }
            }
        }
        
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