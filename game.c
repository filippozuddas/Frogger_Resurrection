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
    int startx = (COLS - GAME_WIDTH) / 2;
    int starty = (LINES - GAME_HEIGHT) / 2;

    if (startx < 0 || starty < 0) {
        endwin();
        printf("Il terminale è troppo piccolo per eseguire il gioco.\n");
        exit(1);
    }


    game->gameWin = newwin(GAME_HEIGHT, GAME_WIDTH, starty, startx);

    disegna_mappa(game->gameWin);
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

    Frog *frog = &game->frog;
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
                frog->info = info;
            }
            /* ID tra 1 e 16 corrispondono ai coccodrilli */
            else if(info.ID >= 1 && info.ID <= 16) {
                croc[info.ID - 1].info = info; 

                if (playerCrocIdx == info.ID && frog->isOnCroc){
                    frog->info.x = info.x + frog->onCrocOffset;
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
        write(game->mainToFrogPipe[1], &frog->info, sizeof(Informations));

        /* Verifico se la rana è caduta in acqua */
        if((frog->isOnCroc == 0 && isFrogOnRiver(game)) || 
            frog->info.x < 0 || frog->info.x + FROG_LENGTH > GAME_WIDTH) {
            if(frog->lives == 0) {
                game->isRunning = 0; 
                break;
            }
            //sleep(1);
            //resetCroc(croc);
            //write(game->mainToCrocPipe[1], &croc->info, sizeof(Informations));
            frog->lives--; 
            frog->info.x = ((GAME_WIDTH - 1) / 2) - 4; 
            frog->info.y = GAME_HEIGHT - 5;
        }

        /* Stampo le entità di gioco */
        werase(game->gameWin); 

        disegna_mappa(game->gameWin);

        mvwprintw(game->gameWin, 1, 1, "Player's lives: %d", frog->lives);
        mvwprintw(game->gameWin, 1, 20, "LINES: %d, COLS: %d", GAME_HEIGHT, GAME_WIDTH);
        mvwprintw(game->gameWin, 1, 50, "x = %d, y = %d", frog->info.x, frog->info.y);
        mvwprintw(game->gameWin, 1, 70, "Grenates remaining: %d", frog->info.grenadesRemaining);

        for (int i = 0; i < N_CROC; i++) {
            printCroc(game->gameWin, croc[i].info.x, croc[i].info.y, croc[i].info.direction);
        }

        printFrog(game->gameWin, frog->info.x, frog->info.y);
        
        for (int i = 0; i < MAX_GRENADES; i++) {
            if (grenades[i].info.ID != -1) {
                mvwprintw(game->gameWin, grenades[i].info.y, grenades[i].info.x, "%s", "💣");
                /* 
                 * Se la granata è uscita dallo schermo, libero lo slot rendendolo disponibile
                 * per un altra granata 
                 */
                if (grenades[i].info.x < -1 || grenades[i].info.x >= COLS) {
                    //kill(grenades[i].info.pid, SIGTERM); 
                    waitpid(grenades[i].info.pid, NULL, 0); 
                    grenades[i].info.ID = -1;
                }
            }
        }
        
        wrefresh(game->gameWin);

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

    delwin(game->gameWin);
    endwin(); 
}