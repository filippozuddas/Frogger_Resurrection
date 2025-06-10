#include "game.h"    

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
    
    //printf("runGame: isRunning = %d\n", game->isRunning); // Verifica il valore di isRunning

    createCroc(game);  // Create crocodile threads
    createFrog(game);  // Create frog thread
    //createTimer(game);

    
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
    
    while (game->isRunning) {
        
        //usleep(6000);  // Controllo del timer ogni 6 millisecondi
        
        
        //------------------------- LETTURA DA MAIN BUFFER PER IDENTIFICARE OGGETTO ----------------------------------
        
         
        // 1. Memorizza la posizione della rana all'inizio del frame
        int old_frog_x = frog->info.x;

        // 2. Raccogli tutti gli input (dal client e dai thread)
        if (receiveInfo(game_socket_fd, &socketInfo) > 0) {
            if (socketInfo.ID == 0) {
                frog->info = socketInfo;
            }
        }
        // Svuota il buffer per evitare lag dei coccodrilli
        while (readMain(&info)) {
            if (info.ID >= 1 && info.ID <= N_CROC) {
                croc[info.ID - 1].info = info;
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

        //writeProd(frog->info);
        
        /* ID tra 27 e 46 corrispondono alle granate della rana */
        // else if (info.ID > N_CROC && info.ID < 57){
            //    for (int i = 0; i < MAX_GRENADES; i++) {
                //        if (grenades[i].info.ID == info.ID) {
                    //            // grenades[i].info.x = info.x; //Aggiorno solo x e y
        //            // grenades[i].info.y = info.y;
        //            grenades[i].info = info;
        //            //fprintf(stderr, "[PADRE] Ricevuto granata - ID: %d, X: %d, Y: %d, PID: %d\n", info.ID, info.x, info.y, grenades[i].info.pid); // DEBUG
        //            break;
        //        }
        //    }
        // }
        
        //pthread_mutex_unlock(&buffer_mutex);
        //writeProd(frog->info);
        // /* ID -1 e -2 usati come flag per creare le granate, rispettivamente destra e sinistra */
        // else if (info.ID == -1 || info.ID == -2) {
        //    int direction = (info.ID == -1) ? 1 : -1;
        //    //createGrenade(game, direction); 
        // }
        // else if (info.ID > 56) {
        //    int foundProjectile = 0;
        //    for (int i = 0; i < MAX_PROJECTILES; i++) {
        //        if (projectiles[i].info.ID == info.ID) {
        //            // projectiles[i].info.x = info.x;
        //            // projectiles[i].info.y = info.y;
        //            projectiles[i].info = info;
        //            //fprintf(stderr, "[PADRE] Ricevuto proiettile - ID: %d, X: %d, Y: %d, PID: %d\n", info.ID, info.x, info.y, projectiles[i].info.pid); // DEBUG
        //            foundProjectile = 1;
        //            break;
            
        //        }
        //    }
        // }
        // pthread_mutex_unlock(&ncurses_mutex);
        /* AGGIUNGERE CONTROLLI E COLLISIONI !!!!!!!!!!!!!!! */
        
        
        pthread_mutex_lock(&ncurses_mutex);
        werase(game->gameWin);
        disegna_mappa(game);
        
        wattron(game->gameWin, A_BOLD); 
        mvwprintw(game->gameWin, 1, 10, "Score: %d", frog->score);
        mvwprintw(game->gameWin, 1, 70, "Grenates remaining: %d", frog->info.grenadesRemaining);
        mvwprintw(game->gameWin, 3, 10, "info.ID = %d", info.ID); 
        mvwprintw(game->gameWin, 3, 50, "playerCrocID = %d", playerCrocID);
        mvwprintw(game->gameWin, 3, 70, "frog->isOnCroc = %d", frog->isOnCroc);
        wattroff(game->gameWin, A_BOLD);
        for (int i = 0; i < N_CROC; i++) {
            if (croc[i].info.active) {
                printCroc(game->gameWin, croc[i].info.x, croc[i].info.y, croc[i].info.direction);
            }
        }
        
        printFrog(game, game->gameWin, frog->info.x, frog->info.y);
        wrefresh(game->gameWin);
        pthread_mutex_unlock(&ncurses_mutex);
        
        usleep(2000);
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
      
        deallocate_synchro();
        
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
    
    void restartMenu(Game *game, int score, int flag) {
    
        pthread_mutex_lock(&ncurses_mutex);
        init_pair(1, COLOR_RED, COLOR_BLACK);
        werase(game->gameWin);
        wrefresh(game->gameWin);
        pthread_mutex_unlock(&ncurses_mutex);
        usleep(10000);
        
        int highlight = 1;
        int choice = 0;
        int c;
        
        // Stampa GAME OVER o YOU WON
        if (flag == 0) {
            printGameOver(game->gameWin);
        } else if (flag == 1) {
            printYouWon(game->gameWin);
        }
    
        // Calcola le posizioni per centrare il punteggio
        int centerX = 72;
        int scoreY = GAME_HEIGHT - 22;  // Posizione Y dello score
        int n_choices = sizeof(restart) / sizeof(MenuOption);
    
        // Stampa il punteggio grande
        digitsAnalyser(game->gameWin, score, scoreY, centerX);
        
        pthread_mutex_lock(&ncurses_mutex);
        // Verifica e aggiorna il display
        wrefresh(game->gameWin);
        pthread_mutex_unlock(&ncurses_mutex);
    
        addScore(&scoreList, game->frog.score);
        
        // Setup menu
        pthread_mutex_lock(&ncurses_mutex);
        keypad(game->gameWin, TRUE);
        
        print_menu(game->gameWin, highlight, restart, n_choices);
        wrefresh(game->gameWin);
        pthread_mutex_unlock(&ncurses_mutex);
    
        // Loop del menu
        pthread_mutex_lock(&ncurses_mutex);
        while ((c = wgetch(game->gameWin)) != 0) {
            pthread_mutex_unlock(&ncurses_mutex);
            switch (c) {
                case KEY_UP:
                    highlight = (highlight == 1) ? n_choices : highlight - 1;
                    break;
                case KEY_DOWN:
                    highlight = (highlight == n_choices) ? 1 : highlight + 1;
                    break;
                case 10:  // Invio
                    choice = highlight;
                    break;
                    // if (choice == 1) {
                    //     pthread_mutex_lock(&ncurses_mutex);
                    //     wclear(game->gameWin);
                    //     wrefresh(game->gameWin);
                    //     pthread_mutex_unlock(&ncurses_mutex);
                    //     initGame(game);
                    //     runGame(game, game_socket_fd);
                    //     stopGame(game);
                    //     return;
                    // } else if (choice == 2) {
                    //     pthread_mutex_lock(&ncurses_mutex);
                    //     wclear(game->gameWin);
                    //     wrefresh(game->gameWin);
                    //     pthread_mutex_unlock(&ncurses_mutex);
                    //     mainMenu(game);
                    //     return;
                    // }
                default:
                    break;
            }
            pthread_mutex_lock(&ncurses_mutex);
            print_menu(game->gameWin, highlight, restart, n_choices);
            wrefresh(game->gameWin);
            pthread_mutex_unlock(&ncurses_mutex);
            if (choice != 0)
                break;
        }
    }
    
    
    
    
    void drawLives(WINDOW *win, int lives) {
        // Calcola la posizione centrale per i cuori
        int heartsWidth = lives * 2;  // Ogni cuore occupa 2 spazi
        int startX = (GAME_WIDTH - heartsWidth) / 2;
        pthread_mutex_lock(&ncurses_mutex);
        // Posiziona i cuori sopra la barra del tempo
        wmove(win, 1, startX);
        wattron(win, COLOR_PAIR(RED_HEARTS));  // Usa il colore rosso per i cuori
        
        for (int i = 0; i < lives; i++) {
            wprintw(win, "♥ ");  // Stampa un cuore con uno spazio
        }
        
        wattroff(win, COLOR_PAIR(RED_HEARTS));
        pthread_mutex_unlock(&ncurses_mutex);
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
    

void* timerThread(void* arg) {


    Game* game = (Game*)arg;
    int countdownTime = game->difficulty == 0 ? 60 : (game->difficulty == 1 ? 45 : 30);
    int timerMax = countdownTime;
    int millisecondCounter = 0;

    while (game->isRunning) {
        usleep(5000);
        millisecondCounter += 5;

        if (millisecondCounter >= 1000) {
            millisecondCounter = 0;

            // Blocca il mutex prima di accedere a game->isRunning
            pthread_mutex_lock(&buffer_mutex);

            countdownTime--;
            if (countdownTime <= 0) {
                countdownTime = 0;
                game->isRunning = 0;
                
                // Sblocca il mutex prima di uscire
                pthread_mutex_unlock(&buffer_mutex); 
                pthread_exit(NULL);
            }

            // Sblocca il mutex dopo l'accesso
            pthread_mutex_unlock(&buffer_mutex); 
        }

        pthread_mutex_lock(&buffer_mutex);
        game->timer.countdownTime = countdownTime;
        game->timer.millisecondCounter = millisecondCounter;
        game->timer.timerMax = timerMax;
        pthread_mutex_unlock(&buffer_mutex);
    }

    pthread_exit(NULL);
}

void createTimer(Game* game) {

    int result = pthread_create(&game->timer.thread, NULL, timerThread, (void*)game);
    if (result != 0) {
        perror("Errore in pthread_create() in createTimer()");
    }

}