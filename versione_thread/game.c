#include "game.h"    
#include "frog.h"

ScoreNode* scoreList = NULL;


void initDens(Game *game) {
    // initDens only calculates positions based on constants. No ncurses, no shared data.
    int denWidth = FROG_WIDTH;
    int denHeight = FROG_HEIGHT;
    int spacing = (GAME_WIDTH - N_DENS * denWidth) / (N_DENS + 1);
    
    for (int i = 0; i < N_DENS; i++) {
        game->dens[i].x = spacing + i * (denWidth + spacing);
        game->dens[i].y = 5;
        game->dens[i].width = denWidth;
        game->dens[i].height = denHeight;
        game->dens[i].isOpen = 1; // 1 = open, 0 = closed
    }
}

void initGame(Game *game) {
    
    init_synchro();
    
    srand(time(NULL)); // Seed random number generator
    
    const int startx = (COLS - GAME_WIDTH) / 2;
    const int starty = (LINES - GAME_HEIGHT) / 2;
    
    
    if (startx < 0 || starty < 0) {
        endwin();
        fprintf(stderr, "Terminal too small.\n");
        exit(1);
    }
    
    // 1. Create the game window *FIRST*
    game->gameWin = newwin(GAME_HEIGHT, GAME_WIDTH, starty, startx);
    if (game->gameWin == NULL) {
        endwin();
        perror("newwin failed"); // ALWAYS check for errors
        exit(1);
    }    

    nodelay(game->gameWin, TRUE);
    keypad(game->gameWin, TRUE); 
    
    // 2. Initialize and draw the map *IMMEDIATELY* after window creation
    inizializza_mappa(); // Sets up color pairs (must be inside ncurses_mutex)
    initDens(game);        // Calculate den positions (no ncurses calls)
    //disegna_mappa(game);  // Draws the map to game->gameWin (inside ncurses_mutex)

    /* Imposta la musica di gioco in base alla difficoltà */
    switch (game->difficulty) {
        case 0: startMusic("../music/LIVELLO1(130BPM).wav"); break;
        case 1: startMusic("../music/LIVELLO2(145BPM).wav"); break;
        case 2: startMusic("../music/LIVELLO3(170BPM).wav"); break;
        default: startMusic("../music/LIVELLO1(130BPM).wav"); break;
    }

    // 3. Initialize game entities (grenades and projectiles - no thread creation yet)
    for (int i = 0; i < MAX_GRENADES; i++) {
        game->grenades[i].info.ID = -1;
        game->grenades[i].info.active = 0;
    }
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        game->projectiles[i].info.ID = -1;
        game->projectiles[i].info.active = 0;
    }
    
    game->isRunning = 1;
}


void runGame(Game* game, int game_socket_fd) {
    
    createCroc(game);  // Create crocodile threads
    createFrog(game);  // Create frog thread

    
    Frog *frog = &game->frog;
    Crocodile *croc = game->crocodile;
    Projectile *projectiles = game->projectiles;
    Grenade *grenades = game->grenades;
  
    Informations info;
    Informations socketInfo;
    
    
    int playerCrocID = 0;
    int grenadeID = N_CROC + 1; 
    int countdownTime = 90; // Tempo iniziale per il countdown in secondi    

    // Imposta il tempo di gioco in base alla difficoltà
    switch (game->difficulty) {
        case 0: countdownTime = 60; break;
        case 1: countdownTime = 45; break;
        case 2: countdownTime = 30; break;
        default: countdownTime = 60; break; 
    }

    int millisecondCounter = 0;
    int timerMax = countdownTime;  // Impostiamo il valore massimo per la barra
    
    while (game->isRunning) {
        
        usleep(6000);  // Controllo del timer ogni 6 millisecondi
        
        countdownTime = timerHandler(game, &millisecondCounter, countdownTime, timerMax); 

        if (countdownTime <= 0) {
            frog->lives--; 
            frog->info.x = ((GAME_WIDTH - 1) / 2) - 4; 
            frog->info.y = GAME_HEIGHT - 5;
            frog->info.grenadesRemaining = 10;
            
            // Reset del timer quando la rana muore
            countdownTime = timerMax;  
            millisecondCounter = 0; 
            
            if (frog->lives == 0) {
                stopMusic();
                int isDead = 1; 
                handleScores(game, countdownTime, isDead);
                game->isRunning = 0; 

                break;
            }

            terminateGrenades(game);
            terminateProjectiles(game);
            resetCroc(game); 

            continue;
        }

        
        //------------------------- LETTURA DA MAIN BUFFER PER IDENTIFICARE OGGETTO ----------------------------------
        
         
        // 1. Memorizza la posizione della rana all'inizio del frame
        int old_frog_x = frog->info.x;

        // 2. Raccogli tutti gli input (dal client e dai thread)
        if (receiveInfo(game_socket_fd, &socketInfo) > 0) {
            if (socketInfo.ID == 0) {
                frog->info = socketInfo;
            }
            else if (socketInfo.ID == -1 || socketInfo.ID == -2) {
                if (frog->info.grenadesRemaining > 0) {
                    writeMain(socketInfo);
                    frog->info.grenadesRemaining--;
                }
            }
        }
        // Svuota il buffer per evitare lag dei coccodrilli
        while (readMain(&info)) {
            if (info.ID >= 1 && info.ID <= N_CROC) {
                croc[info.ID - 1].info = info;
            }

            /* ID -1 e -2 usati come flag per creare le granate, rispettivamente destra e sinistra */
            else if (info.ID == -1 || info.ID == -2) {
                int direction = (info.ID == -1) ? 1 : -1; 
                int grenadeIndex = -1; 

                /* Trovo uno slot dell'array libero */
                for (int i = 0; i < MAX_GRENADES; i++) {
                    if (!grenades[i].info.active) {
                        grenadeIndex = i; 
                        break;
                    }
                }

                if (grenadeIndex != -1) {
                    createGrenade(game, direction, grenadeID++, grenadeIndex); 
                }
            }

            /* ID tra 26 e 56 corrispondono alle granate della rana */
            else if (info.ID > N_CROC && info.ID <= 56){
                for (int i = 0; i < MAX_GRENADES; i++) {
                    if (grenades[i].info.ID == info.ID) {
                        grenades[i].info = info;
                        break;
                    }
                }
            }

            else if (info.ID > 56) {
                for (int i = 0; i < MAX_PROJECTILES; i++) {
                    if (projectiles[i].info.ID == info.ID) {
                        projectiles[i].info = info;
                        break;
                    }
                }
            }
        }

        handleProjectileGeneration(game);

        /* Rilevamento collisione Rana-proiettile */
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].info.active) {
                if (checkCollisionProjectile(frog->info, projectiles[i])) {
                    frog->lives--;
                    frog->info.x = ((GAME_WIDTH - 1) / 2) - 4;
                    frog->info.y = GAME_HEIGHT - 5;
                    frog->info.grenadesRemaining = 10;
                    
                    // Reset del timer quando la rana muore
                    countdownTime = timerMax;  
                    millisecondCounter = 0; 
                    
                    if (frog->lives == 0) {
                        stopMusic();
                        int isDead = 1;
                        handleScores(game, countdownTime, isDead);
                        game->isRunning = 0; 

                        break;
                    }

                    projectiles->info.active = 0;
                    
                    terminateGrenades(game);
                    terminateProjectiles(game);
                    killCroc(game);
                    createCroc(game);

                    continue;
                }
            }
        }

        /* Rilevamento collisione granata-proiettile */
        for (int i = 0; i < MAX_GRENADES; i++) {
            if (grenades[i].info.active == 1) { // Se la granata è attiva
                for (int j = 0; j < MAX_PROJECTILES; j++) {
                    if (projectiles[j].info.active == 1) { // Se il proiettile è attivo
                        if (checkCollisionProjectile(grenades[i].info, projectiles[j])) {
                            // Collisione!
                        
                            pthread_cancel(game->grenades[i].thread);
                            pthread_join(game->grenades[i].thread, NULL);
                            grenades[i].info.active = 0;          // Libera lo slot granata

                            pthread_cancel(game->projectiles[j].thread);
                            pthread_join(game->projectiles[j].thread, NULL);
                            projectiles[j].info.active = 0;        // Libera lo slot proiettile
                            break; 
                        }
                    }
                }
            }
        }

        // 3. Applica la logica di gioco
        playerCrocID = isFrogOnCroc(game);

        // --- LOGICA CHIAVE MODIFICATA ---

        // A. Se il giocatore ha mosso la rana in questo frame...
        if (frog->isOnCroc && (frog->info.x != old_frog_x)) {
            // aggiorniamo l'offset per riflettere questo nuovo posizionamento relativo
            // Calcoliamo di quanto si è mossa la rana e lo aggiungiamo all'offset.
            frog->onCrocOffset += (frog->info.x - old_frog_x);
        }
        
        // B. Applica il movimento passivo del coccodrillo usando l'offset (potenzialmente appena aggiornato)
        if (frog->isOnCroc) {
            frog->info.x = croc[playerCrocID - 1].info.x + frog->onCrocOffset;
        }

        // 4. Invia lo stato finale al client e stampa
        sendInfo(game_socket_fd, &frog->info);

        if (isFrogOnDen(game)) {
            frog->score += 100; 
            frog->info.grenadesRemaining = 10;
            frog->info.x = ((GAME_WIDTH - 1) / 2) - 4;
            frog->info.y = GAME_HEIGHT - 5;

            pthread_mutex_lock(&ncurses_mutex);
            wrefresh(game->gameWin);
        
            // Reset del timer quando la rana raggiunge una tana
            countdownTime = timerMax;  
            millisecondCounter = 0;  
        
            int allDensClosed = 1; 
            for (int i = 0; i < N_DENS; i++) {
                if (game->dens[i].isOpen) {
                    allDensClosed = 0; 
                    break;
                }
            }
        
            if (allDensClosed) {
                int isDead = 0; 
                handleScores(game, countdownTime, isDead);
                werase(game->gameWin);
                disegna_mappa(game);
                wrefresh(game->gameWin);
                stopMusic();
                game->isRunning = 0; 

                break; 
            }

            pthread_mutex_unlock(&ncurses_mutex);

            terminateGrenades(game);
            terminateProjectiles(game);
            killCroc(game);
            createCroc(game);

            for (int i = 0; i < N_CROC; i++) {
                game->crocodile[i].info.active = 0;
            }   
        }

        if((frog->isOnCroc == 0 && isFrogOnRiver(game)) || 
            isFrogOnTopRiver(game) ||
            frog->info.x < 0 || frog->info.x + FROG_WIDTH > GAME_WIDTH) {
            
            frog->lives--; 
            frog->info.x = ((GAME_WIDTH - 1) / 2) - 4; 
            frog->info.y = GAME_HEIGHT - 5;
            frog->info.grenadesRemaining = 10;
            
            // Reset del timer quando la rana muore
            countdownTime = timerMax;  
            millisecondCounter = 0; 
            
            if (frog->lives == 0) {
                stopMusic();
                int isDead = 1; 
                handleScores(game, countdownTime, isDead);
                game->isRunning = 0; 

                break;
            }

            terminateGrenades(game);
            terminateProjectiles(game);
            killCroc(game);
            createCroc(game);

            continue;
        } 
        
        werase(game->gameWin);
        disegna_mappa(game);
        
        wattron(game->gameWin, A_BOLD); 
        mvwprintw(game->gameWin, 1, 10, "Score: %d", frog->score);
        mvwprintw(game->gameWin, 1, 70, "Grenates remaining: %d", frog->info.grenadesRemaining);
        wattroff(game->gameWin, A_BOLD);

        drawLives(game->gameWin, frog->lives);     
        drawTimer(game, game->gameWin, countdownTime, timerMax);

        for (int i = 0; i < N_CROC; i++) {
            if (croc[i].info.active) {
                printCroc(game->gameWin, croc[i].info.x, croc[i].info.y, croc[i].info.direction);
            }
        }
        
        printFrog(game, game->gameWin, frog->info.x, frog->info.y);
        printGrenades(game);
        printProjectiles(game);
        wrefresh(game->gameWin);
        
        usleep(10000);
    }
}
       
void stopGame(Game *game) {
    // Terminate all active threads (projectiles, grenades, crocodiles)
    for(int i = 0; i < MAX_PROJECTILES; i++) {
        if(game->projectiles[i].info.ID != -1) {
            pthread_cancel(game->projectiles[i].thread);
            pthread_join(game->projectiles[i].thread, NULL);
        }
    }
    for(int i = 0; i < MAX_GRENADES; i++) {
        if(game->grenades[i].info.ID != -1) {
            pthread_cancel(game->grenades[i].thread);
            pthread_join(game->grenades[i].thread, NULL);
        }
    }
    for(int i = 0; i < N_CROC; i++) {
        if(game->crocodile[i].info.ID != -1){
             pthread_cancel(game->crocodile[i].thread);
            pthread_join(game->crocodile[i].thread, NULL);
        }
    }

    werase(game->gameWin); 

	if (game->frog.lives == 0 || timerHandler(&game, NULL, 0, 0) <= 0) {
        printGameOver(game->gameWin);
        startMusic("../music/GAMEOVER.wav");
    }
    else {
        printYouWon(game->gameWin); 
        startMusic("../music/YOUWIN.wav");
    } 

    digitsAnalyser(game->gameWin, game->frog.score, GAME_HEIGHT/2, GAME_WIDTH/2);

    wattron(game->gameWin, A_BOLD);
    mvwprintw(game->gameWin, GAME_HEIGHT - 25, (GAME_WIDTH / 2) - 22, "PREMI UN TASTO PER TORNARE AL MENU PRINCIPALE");
    wattroff(game->gameWin, A_BOLD); 
    wrefresh(game->gameWin); 
    int ch; 
    while((ch = wgetch(game->gameWin)) == ERR); 

      stopMusic();
    deallocate_synchro();
	
    delwin(game->gameWin);
}
    
    
void printGameOver(WINDOW *win) {
    const wchar_t *pattern[] = {
        L" ░▒▓██████▓▒░ ░▒▓██████▓▒░░▒▓██████████████▓▒░░▒▓████████▓▒░       ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░▒▓███████▓▒░  ",
        L"░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░             ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ",
        L"░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░             ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ",
        L"░▒▓█▓▒▒▓███▓▒░▒▓████████▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓██████▓▒░        ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒▒▓█▓▒░░▒▓██████▓▒░ ░▒▓███████▓▒░  ",
        L"░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░             ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▓█▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ ",
        L" ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░       ░▒▓██████▓▒░   ░▒▓██▓▒░  ░▒▓████████▓▒░▒▓█▓▒░░▒▓█▓▒░ "
    };
    pthread_mutex_lock(&ncurses_mutex);
    wattron(win, COLOR_PAIR(RED_HEARTS));
    for (int i = 0; i < 6; i++) {
        mvwaddwstr(win, i+20, (GAME_WIDTH - 121) / 2, pattern[i]);  // Spostato in basso
    }
    wattroff(win, COLOR_PAIR(RED_HEARTS));
    pthread_mutex_unlock(&ncurses_mutex);
}


void printYouWon(WINDOW *win){
    const wchar_t *pattern[] = {
        L"░▒▓█▓▒░░▒▓█▓▒░░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓███████▓▒░░▒▓█▓▒░",
        L"░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░",
        L"░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░",
        L" ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░",
        L"   ░▒▓█▓▒░   ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░      ",
        L"   ░▒▓█▓▒░    ░▒▓██████▓▒░ ░▒▓██████▓▒░        ░▒▓█████████████▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░ "
 };

    pthread_mutex_lock(&ncurses_mutex);
    wattron(win, COLOR_PAIR(RED_HEARTS));
    for (int i = 0; i < 3; i++) {
        mvwaddwstr(win, i+20, (GAME_WIDTH - 93) / 2, pattern[i]);  // Spostato più in basso
    }
    wattroff(win, COLOR_PAIR(RED_HEARTS));
    pthread_mutex_unlock(&ncurses_mutex);
}


void drawLives(WINDOW *win, int lives) {
    // Calcola la posizione centrale per i cuori
    int heartsWidth = lives * 2;  // Ogni cuore occupa 2 spazi
    int startX = (GAME_WIDTH - heartsWidth) / 2;
    // Posiziona i cuori sopra la barra del tempo
    wmove(win, 1, startX);
    wattron(win, COLOR_PAIR(RED_HEARTS));  // Usa il colore rosso per i cuori
    
    for (int i = 0; i < lives; i++) {
        wprintw(win, "♥ ");  // Stampa un cuore con uno spazio
    }
    
    wattroff(win, COLOR_PAIR(RED_HEARTS));
}
    
void drawTimer(Game *game, WINDOW *win, int timeLeft, int timerMax) {
    int progressBarWidth = GAME_WIDTH - 9;  // Larghezza della barra
    int filledBlocks = (timeLeft * progressBarWidth) / timerMax;  // Blocchi pieni in base al tempo rimanente
    int remainingBlocks = progressBarWidth - filledBlocks;  // Blocchi vuoti

    pthread_mutex_lock(&ncurses_mutex);
    // Posizioniamo il cursore per disegnare la barra
    wmove(win, 3, 5);
    wprintw(win, "[");
    
    // Disegniamo i blocchi pieni (verde) che rappresentano il tempo che è passato
    wattron(win, COLOR_PAIR(GREEN_BLACK));  
    for (int i = 0; i < filledBlocks; i++) {
        wprintw(win, "█");
    }
    wattroff(win, COLOR_PAIR(GREEN_BLACK));  
    
    // Disegniamo i blocchi vuoti (rosso) che rappresentano il tempo rimanente
    wattron(win, COLOR_PAIR(COLOR_RED));  
    for (int i = 0; i < remainingBlocks; i++) {
        wprintw(win, "░");
    }
    wattroff(win, COLOR_PAIR(COLOR_RED));  
    
    wprintw(win, "]");
    pthread_mutex_unlock(&ncurses_mutex);    
}
    

int timerHandler(Game *game, int *millisecondCounter, int countdownTime, int timerMax) {
    *millisecondCounter += 5;  // Incrementa il contatore del millisecondo di 5

    if (*millisecondCounter >= 1000) {  // Ogni secondo
        *millisecondCounter = 0;  // Reset del contatore dei millisecondi
        countdownTime--;  // Diminuisce il tempo rimasto
    }
    return countdownTime;  // Ritorna il tempo rimanente
}

