#include "game.h"
#include <signal.h>
#include <fcntl.h>

void initDens(Game *game) {
    int denWidth = FROG_WIDTH; 
    int denHeight = FROG_HEIGHT; 
    int spacing = (GAME_WIDTH - N_DENS * denWidth) / (N_DENS + 1); 

    for (int i = 0; i < N_DENS; i++) {
        game->dens[i].x = spacing + i * (denWidth + spacing);
        game->dens[i].y = 7; 
        game->dens[i].width = denWidth;
        game->dens[i].height = denHeight;
        game->dens[i].isOpen = 1; // 1 = aperta, 0 = chiusa
    }
}


void initGame(Game *game) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();
    //use_default_colors();

    setColors(); 
    srand(time(NULL)); 

    game->isRunning = 1; 

    if(pipe(game->pipeFd) < 0 || pipe(game->mainToFrogPipe) < 0) {
        perror("pipe creation error"); 
        exit(1); 
    }

    setNonBlocking(game->mainToFrogPipe[0]);
    setNonBlocking(game->mainToFrogPipe[1]);

    setNonBlocking(game->pipeFd[0]);  // Lettura
    setNonBlocking(game->pipeFd[1]);  // Scrittura

    inizializza_mappa();

    int startx = (COLS - GAME_WIDTH) / 2;
    int starty = (LINES - GAME_HEIGHT) / 2;

    if (startx < 0 || starty < 0) {
        endwin();
        printf("Il terminale è troppo piccolo per eseguire il gioco.\n");
        exit(1);
    }
    initDens(game); 


    game->gameWin = newwin(GAME_HEIGHT, GAME_WIDTH, starty, startx);

    disegna_mappa(game);
}


void runGame(Game* game) {

    /* Creazione e inizializzazione dei coccodrilli e della rana (player) */
    createCroc(game);
    createFrog(game);

    
    /*
     * Inizializzazione dell'array di granate e dei proiettili
     * ID = -1 indica che nessuna granata è associata a quell'elemento dell'array
     */
    for (int i = 0; i < MAX_GRENADES; i++) {
        game->grenades[i].info.ID = -1;  
    }
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        game->projectiles[i].info.ID = -1;
    }
    

    /*
     * Per comodità creo dei puntatori per gestire le entità dinamiche, 
     * senza dover scrivere ogni volta 'game->...'
     */

    Frog *frog = &game->frog;
    Crocodile *croc = game->crocodile;
    Grenade *grenades = game->grenades;
    Projectile *projectiles = game->projectiles; 
    
    /*
     * Struttura Informations per memorizzare i dati (coordinate e altre info) letti dalla pipe, 
     * provenienti dalle varie entità di gioco dinamiche 
     */
    Informations info;

    /* Chiudo i lati delle pipe inutilizzati */
    close(game->pipeFd[1]);
    close(game->mainToFrogPipe[0]);

    /* Indice del del coccodrillo sul quale si trova la rana */
    int playerCrocIdx = 0;

    int projectileID = 27; 

    /* Ciclo principale di gestione del gioco */
    while (game->isRunning) {
        /* Lettura dalla pipe */
        while(full_read(game->pipeFd[0], &info, sizeof(Informations)) > 0){
            /* ID = 0 corrisponde alla rana */
            if (info.ID == 0) {
                frog->info = info;
            }
            /* ID tra 1 e 16 corrispondono ai coccodrilli */
            else if (info.ID >= 1 && info.ID <= N_CROC) {
                croc[info.ID - 1].info = info; 

                if (playerCrocIdx == info.ID && frog->isOnCroc){
                    frog->info.x = info.x + frog->onCrocOffset;
                }
            }
            /* ID tra 17 e 26 corrispondono alle granate della rana */
            else if (info.ID > N_CROC && info.ID <= 26){
                int foundGrenade = 0;
                for (int i = 0; i < MAX_GRENADES; i++) {
                    if (grenades[i].info.ID == info.ID) {
                        grenades[i].info = info;
                        foundGrenade = 1;
                        break;
                    }
                }
                if (!foundGrenade) {
                    // Aggiungi una nuova granata
                    for (int i = 0; i < MAX_GRENADES; i++) {
                        if (grenades[i].info.ID == -1) {
                            grenades[i].info = info;
                            break;
                        }
                    }
                }
            }
            /* ID > 26 corrispondono ai proiettili dei coccodrilli */
            else if (info.ID > 26) {
                int foundProjectile = 0;
                for (int i = 0; i < MAX_PROJECTILES; i++) {
                    if (game->projectiles[i].info.ID == info.ID) {
                        game->projectiles[i].info = info;
                        foundProjectile = 1;
                        break;
                    }
                }
                if (!foundProjectile) {
                    for (int i = 0; i < MAX_PROJECTILES; i++) {
                        if (game->projectiles[i].info.ID == -1) {
                            game->projectiles[i].info = info;
                            break;
                        }
                    }
                }
            }
        }

        /*int shootChance = rand() % 100;
        if (shootChance < 10) {
            int idx = rand() % N_CROC + 1; 
            createProjectile(&croc[idx], game->pipeFd, projectileID++);
        }
        handleProjectileGeneration(game);*/
        
        /* Verifico se la rana si trova sopra un coccodrillo */
        playerCrocIdx = isFrogOnCroc(game);
        /* 
         * Attraverso una pipe inversa (dal padre al processo rana) comunico le nuove coordinate
         * alla rana, nel caso in cui sia sopra un coccodrillo  
         */
        flushPipe(game->mainToFrogPipe[1]);
        write(game->mainToFrogPipe[1], &frog->info, sizeof(Informations));

        /* Verifico se la rana è caduta in acqua */
        if((frog->isOnCroc == 0 && isFrogOnRiver(game)) || 
            frog->info.x < 0 || frog->info.x + FROG_WIDTH > GAME_WIDTH) {

            frog->lives--; 
            frog->info.x = ((GAME_WIDTH - 1) / 2) - 4; 
            frog->info.y = GAME_HEIGHT - 5;

            if(frog->lives == 0) {
                game->isRunning = 0; 
                break;
            }

            resetCroc(game); 
            continue;
        }

        /* Verifico se la rana è su una tana */
        if (isFrogOnDen(game)) {
            frog->score += 100; 
            frog->info.grenadesRemaining = 5;
            frog->info.x = ((GAME_WIDTH - 1) / 2) - 4; 
            frog->info.y = GAME_HEIGHT - 5;

            int allDensClosed = 1; 
            for (int i = 0; i < N_DENS; i++) {
                if (game->dens[i].isOpen) {
                    allDensClosed = 0; 
                    break;
                }
            }
            if (allDensClosed) {
                sleep(1);
                werase(game->gameWin);
                mvwprintw(game->gameWin, GAME_HEIGHT/2, GAME_WIDTH/2 - 4, "HAI VINTO");
                wrefresh(game->gameWin);
                sleep(1);
                game->isRunning = 0; 
            }

            resetCroc(game); 
            continue;
        }

        /* Stampo le entità di gioco */
        werase(game->gameWin); 

        disegna_mappa(game);

        mvwprintw(game->gameWin, 1, 1, "Player's lives: %d", frog->lives);
        mvwprintw(game->gameWin, 1, 20, "LINES: %d, COLS: %d", GAME_HEIGHT, GAME_WIDTH);
        mvwprintw(game->gameWin, 1, 50, "x = %d, y = %d", frog->info.x, frog->info.y);
        mvwprintw(game->gameWin, 1, 70, "Grenates remaining: %d", frog->info.grenadesRemaining);

        for (int i = 0; i < N_CROC; i++) {
            printCroc(game->gameWin, croc[i].info.x, croc[i].info.y, croc[i].info.direction);
        }

        printFrog(game, game->gameWin, frog->info.x, frog->info.y);
        
        for (int i = 0; i < MAX_GRENADES; i++) {
            if (grenades[i].info.ID != -1) {
                mvwprintw(game->gameWin, grenades[i].info.y, grenades[i].info.x, "%s", "*");
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

        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (game->projectiles[i].info.ID != -1) {
                mvwprintw(game->gameWin, game->projectiles[i].info.y, game->projectiles[i].info.x, "000");
                /* 
                 * Se la granata è uscita dallo schermo, libero lo slot rendendolo disponibile
                 * per un altra granata 
                 */
                if (game->projectiles[i].info.x < 0 || game->projectiles[i].info.x >= GAME_WIDTH) {
                    waitpid(game->projectiles[i].info.pid, NULL, 0);
                    game->projectiles[i].info.ID = -1; 
                }
            }
        }


        wrefresh(game->gameWin);

        //usleep(1000);
    }
}


void stopGame(Game *game) {
    /* Uccido i processi coccodrillo */ 
    killCroc(game);

    delwin(game->gameWin);
    endwin(); 
}

//accerta che il contenuto scritto nella pipe venga letto detl tutto
ssize_t full_read(int fd, void *buffer, size_t size) {
    size_t totalRead = 0;
    while (totalRead < size) {
        ssize_t bytesRead = read(fd, (char *)buffer + totalRead, size - totalRead);
        if (bytesRead <= 0) {
            return bytesRead; // Errore o pipe chiusa
        }
        totalRead += bytesRead;
    }
    return totalRead;
}


//serve a liberare la pipe prima della scrittura 
void flushPipe(int fd) {
    Informations tmp;
    while (read(fd, &tmp, sizeof(Informations)) > 0);
}

//setta la pipe non bloccante
void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}