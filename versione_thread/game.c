#include "game.h"    
#include "frog.h"

// Definizione delle costanti per gli score
ScoreNode* scoreList = NULL;


// Inizializza le tane della rana. Le posizioni sono calcolate in base a costanti
void initDens(Game *game) {

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

// Inizializza il gioco, creando la finestra di gioco e impostando le condizioni iniziali
void initGame(Game *game) {
    
    init_synchro();
    
    srand(time(NULL));
    
    const int startx = (COLS - GAME_WIDTH) / 2;
    const int starty = (LINES - GAME_HEIGHT) / 2;
    
    
    if (startx < 0 || starty < 0) {
        endwin();
        fprintf(stderr, "Terminal too small.\n");
        exit(1);
    }
    
    // Crea la finestra di gioco
    game->gameWin = newwin(GAME_HEIGHT, GAME_WIDTH, starty, startx);
    if (game->gameWin == NULL) {
        endwin();
        perror("newwin failed"); // ALWAYS check for errors
        exit(1);
    }    

    nodelay(game->gameWin, TRUE);
    keypad(game->gameWin, TRUE); 
    
    // Inizializza le strutture del gioco
    inizializza_mappa();
    initDens(game);       

    // Imposta la musica in base alla difficoltà
    switch (game->difficulty) {
        case 0: startMusic("../music/LIVELLO1(130BPM).wav"); break;
        case 1: startMusic("../music/LIVELLO2(145BPM).wav"); break;
        case 2: startMusic("../music/LIVELLO3(170BPM).wav"); break;
        default: startMusic("../music/LIVELLO1(130BPM).wav"); break;
    }

    // Inizializza granate e proiettili
    for (int i = 0; i < MAX_GRENADES; i++) {
        game->grenades[i].info.ID = -1;
        game->grenades[i].info.active = 0;
    }
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        game->projectiles[i].info.ID = -1;
        game->projectiles[i].info.active = 0;
    }
    
    // Fa partire il gioco
    game->isRunning = 1;
}


// Funzione principale del gioco, gestisce la logica di gioco e le interazioni tra i thread
void runGame(Game* game, int game_socket_fd) {
    
    createCroc(game);  
    createFrog(game); 

    // Crea i puntatori per semplificare l'accesso agli oggetti del gioco
    Frog *frog = &game->frog;
    Crocodile *croc = game->crocodile;
    Projectile *projectiles = game->projectiles;
    Grenade *grenades = game->grenades;
  
    Informations info;
    Informations socketInfo;
    
    
    int playerCrocID = 0;
    int grenadeID = N_CROC + 1; 
    int countdownTime = 90;   

    // Imposta il tempo di gioco in base alla difficoltà
    switch (game->difficulty) {
        case 0: countdownTime = 60; break;
        case 1: countdownTime = 45; break;
        case 2: countdownTime = 30; break;
        default: countdownTime = 60; break; 
    }

    int millisecondCounter = 0;
    int timerMax = countdownTime;  
    
    // Inizializza il gioco
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
            killCroc(game);
            createCroc(game);
            continue;
        }

        
        //------------------------- LETTURA DA MAIN BUFFER PER IDENTIFICARE OGGETTO ----------------------------------
        
         
        // Memorizza la posizione della rana all'inizio del frame
        int old_frog_x = frog->info.x;

        // Raccogle tutti gli input (dal client e dai thread)
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

            // ID -1 e -2 sono utilizzati per le granate lanciate dalla rana
            else if (info.ID == -1 || info.ID == -2) {
                int direction = (info.ID == -1) ? 1 : -1; 
                int grenadeIndex = -1; 

                // Trova un indice libero per la granata
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

            // ID > N_CROC e <= 56 sono utilizzati per le granate lanciate dai coccodrilli
            else if (info.ID > N_CROC && info.ID <= 56){
                for (int i = 0; i < MAX_GRENADES; i++) {
                    if (grenades[i].info.ID == info.ID) {
                        grenades[i].info = info;
                        break;
                    }
                }
            }

            // ID > 56 sono utilizzati per i proiettili lanciati dai coccodrilli
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

        // Rilevamento collisione rana-proiettile
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

        // Rilevamento collisione granata-proiettile
        for (int i = 0; i < MAX_GRENADES; i++) {
            if (grenades[i].info.active == 1) { // Se la granata è attiva
                for (int j = 0; j < MAX_PROJECTILES; j++) {
                    if (projectiles[j].info.active == 1) { // Se il proiettile è attivo
                        if (checkCollisionProjectile(grenades[i].info, projectiles[j])) {

                            // Collisione
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

        playerCrocID = isFrogOnCroc(game);

        if (frog->isOnCroc && (frog->info.x != old_frog_x)) {
            frog->onCrocOffset += (frog->info.x - old_frog_x);
        }
        
        // Applica il movimento passivo del coccodrillo usando l'offset 
        if (frog->isOnCroc) {
            frog->info.x = croc[playerCrocID - 1].info.x + frog->onCrocOffset;
        }

        // Invia lo stato finale al client e stampa
        sendInfo(game_socket_fd, &frog->info);

        // Controlla se la rana è su una tana
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

        // Controlla se la rana è morta
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
        
        // Gestione della visualizzazione del gioco
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
       
// Termina il gioco, cancellando i thread attivi e mostrando il risultato finale
void stopGame(Game *game) {
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

    // Cancella la finestra di gioco e mostra il risultato finale
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
    
    
// Funzione per stampare il messaggio di Game Over
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
        mvwaddwstr(win, i+20, (GAME_WIDTH - 121) / 2, pattern[i]);  
    }
    wattroff(win, COLOR_PAIR(RED_HEARTS));
    pthread_mutex_unlock(&ncurses_mutex);
}


// Funzione per stampare il messaggio di vittoria
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


// Funzione per disegnare i cuori che rappresentano le vite rimanenti
void drawLives(WINDOW *win, int lives) {

    int heartsWidth = lives * 2;  
    int startX = (GAME_WIDTH - heartsWidth) / 2;

    wmove(win, 1, startX);
    wattron(win, COLOR_PAIR(RED_HEARTS));  
    
    for (int i = 0; i < lives; i++) {
        wprintw(win, "♥ ");  
    }
    
    wattroff(win, COLOR_PAIR(RED_HEARTS));
}
    
// Funzione per disegnare la barra del timer
void drawTimer(Game *game, WINDOW *win, int timeLeft, int timerMax) {
    int progressBarWidth = GAME_WIDTH - 9;  
    int filledBlocks = (timeLeft * progressBarWidth) / timerMax;  
    int remainingBlocks = progressBarWidth - filledBlocks;  

    pthread_mutex_lock(&ncurses_mutex);

    wmove(win, 3, 5);
    wprintw(win, "[");
    
    // Disegniamo i blocchi pieni (verde) che rappresentano il tempo che è passato
    wattron(win, COLOR_PAIR(GREEN_BLACK));  
    for (int i = 0; i < filledBlocks; i++) {
        wprintw(win, "█");
    }
    wattroff(win, COLOR_PAIR(GREEN_BLACK));  
    
    // Disegniamo i blocchi vuoti che rappresentano il tempo rimanente
    wattron(win, COLOR_PAIR(COLOR_RED));  
    for (int i = 0; i < remainingBlocks; i++) {
        wprintw(win, "░");
    }
    wattroff(win, COLOR_PAIR(COLOR_RED));  
    
    wprintw(win, "]");
    pthread_mutex_unlock(&ncurses_mutex);    
}
    
// Funzione per gestire il timer del gioco, decrementando il tempo rimanente ogni secondo
int timerHandler(Game *game, int *millisecondCounter, int countdownTime, int timerMax) {
    *millisecondCounter += 5;  
    if (*millisecondCounter >= 1000) {  
        *millisecondCounter = 0; 
        countdownTime--; 
    }
    return countdownTime;  
}

