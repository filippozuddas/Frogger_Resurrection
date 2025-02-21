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
}


void runGame(Game* game) {
    /* Creazione e inizializzazione dei coccodrilli e della rana (player) */
    createCroc(game);
    createFrog(game);

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

    close(game->mainToFrogPipe[0]);

    /* Indice del del coccodrillo sul quale si trova la rana */
    int playerCrocIdx = 0;
    int grenadeID = N_CROC + 1; 

    /* Ciclo principale di gestione del gioco */
    while (game->isRunning) {
        /* Gestione delle terminazione dei processi proiettili e granate */
        int status;
        pid_t exited_pid;
        while ((exited_pid = waitpid(-1, &status, WNOHANG)) > 0) {
            // Cerca e libera lo slot del proiettile/granata
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].info.pid == exited_pid) {
                    projectiles[i].info.ID = -1; // Libera lo slot
                    break;
                }
            }
            for (int i = 0; i < MAX_GRENADES; i++) {
                if (grenades[i].info.pid == exited_pid) {
                    grenades[i].info.ID = -1;   // Libera lo slot 
                    break;
                }
            }
        }

        /* Lettura dalla pipe */
        while(readData(game->pipeFd[0], &info, sizeof(Informations)) > 0){
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

            /* ID -1 e -2 usati come flag per creare le granate, rispettivamente destra e sinistra */
            else if (info.ID == -1 || info.ID == -2) {
                int direction = (info.ID == -1) ? 1 : -1; 
                int grenadeIndex = -1; 

                for (int i = 0; i < MAX_GRENADES; i++) {
                    if (grenades[i].info.ID == -1) {
                        grenadeIndex = i; 
                        break;
                    }
                }

                if (grenadeIndex != -1) {
                    createGrenade(game, direction, grenadeID++, grenadeIndex); 
                }
            }

            /* ID tra 17 e 46 corrispondono alle granate della rana */
            else if (info.ID > N_CROC && info.ID < 46){
                int foundGrenade = 0;
                for (int i = 0; i < MAX_GRENADES; i++) {
                    if (grenades[i].info.ID == info.ID) {
                        // grenades[i].info.x = info.x; //Aggiorno solo x e y
                        // grenades[i].info.y = info.y;
                        grenades[i].info = info;
                        //fprintf(stderr, "[PADRE] Ricevuto granata - ID: %d, X: %d, Y: %d, PID: %d\n", info.ID, info.x, info.y, grenades[i].info.pid); // DEBUG
                        foundGrenade = 1;
                        break;
                    }
                }    
            }

            /* ID > 46 corrispondono ai proiettili dei coccodrilli */
            else if (info.ID > 46) {
                int foundProjectile = 0;
                for (int i = 0; i < MAX_PROJECTILES; i++) {
                    if (projectiles[i].info.ID == info.ID) {
                        // projectiles[i].info.x = info.x;
                        // projectiles[i].info.y = info.y;
                        projectiles[i].info = info;
                        //fprintf(stderr, "[PADRE] Ricevuto proiettile - ID: %d, X: %d, Y: %d, PID: %d\n", info.ID, info.x, info.y, projectiles[i].info.pid); // DEBUG
                        foundProjectile = 1;
                        break;
                    }
                }
            }
        }

        handleProjectileGeneration(game);

        /* Rilevamente collisione Rana-Proiettile */
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].info.ID != -1) {
                if (checkCollisionProjectile(frog->info, projectiles[i])) {
                    frog->lives--;
                    frog->info.x = ((GAME_WIDTH - 1) / 2) - 4;
                    frog->info.y = GAME_HEIGHT - 5;
                    frog->info.grenadesRemaining = 5;

                    if (frog->lives == 0) {
                        game->isRunning = 0; 
                        break;
                    }

                    terminateGrenades(game);
                    terminateProjectiles(game);
                    resetCroc(game);

                    continue;
                }
            }
        }

        /* Rilevamento collisione Granata-Proiettile */
        for (int i = 0; i < MAX_GRENADES; i++) {
            if (grenades[i].info.ID != -1) { // Se la granata è attiva
                for (int j = 0; j < MAX_PROJECTILES; j++) {
                    if (projectiles[j].info.ID != -1) { // Se il proiettile è attivo
                        if (checkCollisionProjectile(grenades[i].info, projectiles[j])) {
                            // Collisione!
                            kill(grenades[i].info.pid, SIGKILL); // Termina la granata
                            waitpid(grenades[i].info.pid, NULL, 0);
                            grenades[i].info.ID = -1;          // Libera lo slot granata

                            kill(projectiles[j].info.pid, SIGKILL); // Termina il proiettile
                            waitpid(projectiles[j].info.pid, NULL, 0);
                            projectiles[j].info.ID = -1;        // Libera lo slot proiettile

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
        flushPipe(game->mainToFrogPipe[1]);
        writeData(game->mainToFrogPipe[1], &frog->info, sizeof(Informations));

        /* Verifico se la rana è caduta in acqua */
        if((frog->isOnCroc == 0 && isFrogOnRiver(game)) || 
            frog->info.x < 0 || frog->info.x + FROG_WIDTH > GAME_WIDTH) {

            frog->lives--; 
            frog->info.x = ((GAME_WIDTH - 1) / 2) - 4; 
            frog->info.y = GAME_HEIGHT - 5;
            frog->info.grenadesRemaining = 5;

            if(frog->lives == 0) {
                game->isRunning = 0; 
                break;
            }

            terminateGrenades(game);
            terminateProjectiles(game);

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
            }
        }
        
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].info.ID != -1) {
                mvwprintw(game->gameWin, projectiles[i].info.y, projectiles[i].info.x, "0");
            }
        }
        
        wrefresh(game->gameWin);

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

            terminateGrenades(game); 
            terminateProjectiles(game);

            resetCroc(game); 
        }

        usleep(1000);
    }
}


void stopGame(Game *game) {
    /* Uccido i processi coccodrillo */ 
    killCroc(game);
    // killProjectiles(game);

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