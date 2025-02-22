#include <ncursesw/ncurses.h>
#include "game.h"
#include "menu.h"
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <menu.h>

ScoreNode* scoreList = NULL;


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define RED_HEARTS 29
#define MAX_HEIGHT 6
#define PATTERN_WIDTH 148


void initDens(Game *game) {
    int denWidth = FROG_WIDTH; 
    int denHeight = FROG_HEIGHT; 
    int spacing = (GAME_WIDTH - N_DENS * denWidth) / (N_DENS + 1); 

    for (int i = 0; i < N_DENS; i++) {
        game->dens[i].x = spacing + i * (denWidth + spacing);
        game->dens[i].y = 5; 
        game->dens[i].width = denWidth;
        game->dens[i].height = denHeight;
        game->dens[i].isOpen = 1; // 1 = aperta, 0 = chiusa
    }
}


void initGame(Game *game) {

    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    nodelay(game->gameWin, TRUE);
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

    
    int startx = (COLS - GAME_WIDTH) / 2;
    int starty = (LINES - GAME_HEIGHT) / 2;
    
    if (startx < 0 || starty < 0) {
        endwin();
        printf("Il terminale è troppo piccolo per eseguire il gioco.\n");
        exit(1);
    }
    
    game->gameWin = newwin(GAME_HEIGHT, GAME_WIDTH, starty, startx);
    
    inizializza_mappa();
    initDens(game); 
    disegna_mappa(game);

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

    /* Chiudo i lati delle pipe inutilizzati */
    close(game->mainToFrogPipe[0]);

    /* Indice del del coccodrillo sul quale si trova la rana */
    int playerCrocIdx = 0;
    int grenadeID = N_CROC + 1; 
    int countdownTime = 90; // Tempo iniziale per il countdown in secondi
    int flag ;

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
            /* ID > 26 corrispondono ai proiettili dei coccodrilli */
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
            
            // Reset del timer quando la rana muore
            countdownTime = timerMax;  
            millisecondCounter = 0; 
            
            if(frog->lives == 0) {
                int isDead = 1;
                close(game->pipeFd[0]);
                close(game->mainToFrogPipe[1]);
                handleScores(game, countdownTime, isDead);
                game->isRunning = 0;
                flag = 0;
                restartMenu(game, frog->score, flag); 
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
        mvwprintw(game->gameWin, 1, 70, "Grenates remaining: %d", frog->info.grenadesRemaining);
        mvwprintw(game->gameWin, 1, 10, "Score: %d", frog->score);

        drawLives(game->gameWin, frog->lives);     
        drawTimer(game, game->gameWin, countdownTime, timerMax);  // Chiama la funzione per disegnare la barra del timer
   
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
        
        if (isFrogOnTopBank(game)) {
            frog->score += 50; // Aggiungi punti per aver raggiunto la riva
        }
        
        /* Verifico se la rana è su una tana */
        if (isFrogOnDen(game)) {
            frog->score += 100; 
            frog->info.grenadesRemaining = 5;
            frog->info.x = ((GAME_WIDTH - 1) / 2) - 4;
            frog->info.y = GAME_HEIGHT - 5;
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
                sleep(1);  
                werase(game->gameWin);
                wrefresh(game->gameWin);  
                game->isRunning = 0; 
                flag = 1;
                restartMenu(game,frog->score, flag);
            }

            terminateGrenades(game); 
            terminateProjectiles(game);
        
            resetCroc(game); 
        }
        
        usleep(5000);

    }
}


void stopGame(Game *game) {
    /* Uccido i processi coccodrillo */ 
    killCroc(game);
    delwin(game->gameWin);
}


//serve a liberare la pipe prima della scrittura 
void flushPipe(int fd) {
    Informations tmp;
    while (readData(fd, &tmp, sizeof(Informations)) > 0);
}

//setta la pipe non bloccante
void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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

    werase(win);
    wattron(win, COLOR_PAIR(RED_HEARTS));
    for (int i = 0; i < 6; i++) {
        mvwaddwstr(win, i+20, 25, pattern[i]);  // Spostato in basso
    }
    wattroff(win, COLOR_PAIR(RED_HEARTS));
    wrefresh(win);
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

    werase(win);
    wattron(win, COLOR_PAIR(RED_HEARTS));
    for (int i = 0; i < 3; i++) {
         mvwaddwstr(win, i+20, 25, pattern[i]);  // Spostato più in basso
    }
    wattroff(win, COLOR_PAIR(RED_HEARTS));
    wrefresh(win);
}

void restartMenu(Game *game, int score, int flag) {
    init_pair(1, COLOR_RED, COLOR_BLACK);
    werase(game->gameWin);
    wrefresh(game->gameWin);
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

    // Stampa il punteggio grande
    digitsAnalyser(game->gameWin, score, scoreY, centerX);
    
    // Verifica e aggiorna il display
    wrefresh(game->gameWin);
    addScore(&scoreList, game->frog.score);
    
    // Setup menu
    keypad(game->gameWin, TRUE);
    int n_choices = sizeof(restart) / sizeof(MenuOption);
    print_menu(game->gameWin, highlight, restart, n_choices);
    wrefresh(game->gameWin);

    // Loop del menu
    while ((c = wgetch(game->gameWin)) != 0) {
        switch (c) {
            case KEY_UP:
                highlight = (highlight == 1) ? n_choices : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == n_choices) ? 1 : highlight + 1;
                break;
            case 10:  // Invio
                choice = highlight;
                if (choice == 1) {
                    wclear(game->gameWin);
                    wrefresh(game->gameWin);
                    initGame(game);
                    runGame(game);
                    stopGame(game);
                    return;
                } else if (choice == 2) {
                    wclear(game->gameWin);
                    wrefresh(game->gameWin);
                    mainMenu(game);
                    return;
                }
            default:
                break;
        }
        print_menu(game->gameWin, highlight, restart, n_choices);
        wrefresh(game->gameWin);
        if (choice != 0)
            break;
    }
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
}

int timerHandler(Game *game, int *millisecondCounter, int countdownTime, int timerMax) {
    *millisecondCounter += 5;  // Incrementa il contatore del millisecondo di 5

    if (*millisecondCounter >= 1000) {  // Ogni secondo
        *millisecondCounter = 0;  // Reset del contatore dei millisecondi
        countdownTime--;  // Diminuisce il tempo rimasto

        // Se il tempo è scaduto, termina il gioco
        if (countdownTime <= 0) {
            countdownTime = 0; 
            game->isRunning = 0;  // Ferma il gioco
            int flag = 0;
            restartMenu(game, game->frog.score, flag);
        
        }
    }
    return countdownTime;  // Ritorna il tempo rimanente
}


void handleScores(Game *game, int countdownTime, int isDead) {
    // Penalità per morte veloce
    if (isDead) {
        if (countdownTime >= 40) {
            game->frog.score -= 50;
        } else if (countdownTime >= 20) {
            game->frog.score -= 25;
        } else {
            game->frog.score -= 10;
        }
        if (game->frog.score < 0) game->frog.score = 0; // Evita punteggi negativi
        return; // Esci dalla funzione perché il punteggio per la vittoria non deve essere applicato
    }

    // Punteggio per la vittoria in base al tempo rimanente
    switch (game->difficulty) {
        case 0: // Facile
            if (countdownTime >= 50) game->frog.score += 100;
            else if (countdownTime >= 40) game->frog.score += 75;
            else if (countdownTime >= 30) game->frog.score += 50;
            else if (countdownTime >= 20) game->frog.score += 25;
            else if (countdownTime >= 10) game->frog.score += 10;
            break;
            case 1: // Normale
            if (countdownTime >= 30) game->frog.score += 150;
            else if (countdownTime >= 35) game->frog.score += 100;
            else if (countdownTime >= 25) game->frog.score += 75;
            else if (countdownTime >= 15) game->frog.score += 50;
            else game->frog.score += 10;
            break;
        case 2: // Difficile
            if (countdownTime >= 10) game->frog.score += 200;
            else if (countdownTime >= 30) game->frog.score += 125;
            else if (countdownTime >= 20) game->frog.score += 100;
            else game->frog.score += 50;
            break;
    }

    loadScores(&scoreList);
    
    // Aggiungi il nuovo punteggio
    addScore(&scoreList, game->frog.score);
    
    // Salva tutti i punteggi
    saveScores(scoreList);


}

ScoreNode* createNode(int score) {
    ScoreNode* newNode = (ScoreNode*)malloc(sizeof(ScoreNode));
    newNode->score = score;
    newNode->next = NULL;
    return newNode;
}

void addScore(ScoreNode** head, int score) {
    ScoreNode* newNode = createNode(score);
    newNode->next = *head;  // Il nuovo nodo punta al vecchio head
    *head = newNode;  // Il nuovo nodo diventa la testa della lista
}

void saveScores(ScoreNode* head) {
    FILE* file = fopen(SCORES_FILE, "wb");
    if (file == NULL) {
        perror("Error opening scores file");
        return;
    }
    
    time_t now;
    time(&now);
    struct tm* timeinfo = localtime(&now);
    
    ScoreEntry entry;
    ScoreNode* current = head;
    int count = 0;
    
    while (current != NULL && count < MAX_SCORES) {
        entry.score = current->score;
        strftime(entry.date, sizeof(entry.date), "Y-%m-%d %H:%M:%S", timeinfo);
        
        fwrite(&entry, sizeof(ScoreEntry), 1, file);
        current = current->next;
        count++;
    }
    
    fclose(file);
}

// Funzione per caricare i punteggi dal file
void loadScores(ScoreNode** head) {
    FILE* file = fopen(SCORES_FILE, "rb");
    if (file == NULL) {
        // File non esiste ancora, non è un errore
        return;
    }
    
    // Pulisci la lista esistente
    while (*head != NULL) {
        ScoreNode* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
    
    ScoreEntry entry;
    while (fread(&entry, sizeof(ScoreEntry), 1, file) == 1) {
        addScore(head, entry.score);
    }
    
    fclose(file);
}

void printDigit(WINDOW *win, int digit, int startX, int startY) {
    const wchar_t **digitSprite = NULL;

    const wchar_t *zero[DIGIT_HEIGHT] = {
        L"█▀▀█",  // Prima riga
        L"█▄▀█",  // Seconda riga
        L"█▄▄█"  // Terza rigA 
    };
    const wchar_t *one[DIGIT_HEIGHT] = {
        L"▄█░",  // Prima riga
        L"░█░",  // Seconda riga
        L"▄█▄"  // Terza riga
    };
    
    const wchar_t *two[DIGIT_HEIGHT] = {
        L"█▀█",  // Prima riga
        L"░▄▀",  // Seconda riga
        L"█▄▄"  // Terza riga
    };
    
    const wchar_t *three[DIGIT_HEIGHT] = {
        L"█▀▀█",  // Prima riga
        L"░░▀▄",  // Seconda riga
        L"█▄▄█"  // Terza riga
    };
    const wchar_t *four[DIGIT_HEIGHT] = {
        L"░█▀█░", // Prima riga
        L"█▄▄█▄",  // Seconda riga
        L"░░░█░"  // Terza riga
    };
    
    const wchar_t *five[DIGIT_HEIGHT] = {
        L"█▀▀",  // Prima riga
        L"▀▀▄",  // Seconda riga
        L"▄▄▀"  // Terza riga
    };
    
    const wchar_t *six[DIGIT_HEIGHT] = {
        L"▄▀▀▄",  // Prima riga
        L"█▄▄░",  // Seconda riga
        L"▀▄▄▀"  // Terza riga
    };
    
    const wchar_t *seven[DIGIT_HEIGHT] = {
        L"▀▀▀█",  // Prima riga
        L"░░█░",  // Seconda riga
        L"░▐▌░"  // Terza riga
    };
    const wchar_t *eight[DIGIT_HEIGHT] = {
        L"▄▀▀▄",  // Prima riga
        L"▄▀▀▄",  // Seconda riga
        L"▀▄▄▀"  // Terza riga
    };
    
    const wchar_t *nine[DIGIT_HEIGHT] = {
        L"▄▀▀▄",  // Prima riga
        L"▀▄▄█",  // Seconda riga
        L"░▄▄▀"  // Terza riga
        };    

    // Scegli la sprite del numero
    switch (digit) {
        case 0:
            digitSprite = zero;
            break;
        case 1:
            digitSprite = one;
            break;
        case 2:
            digitSprite = two;
            break;
        case 3:
            digitSprite = three;
            break;
        case 4:
            digitSprite = four;
            break;
        case 5:
            digitSprite = five;
            break;
        case 6:
            digitSprite = six;
            break;
        case 7:
            digitSprite = seven;
            break;
        case 8:
            digitSprite = eight;
            break;
        case 9:
            digitSprite = nine;
            break;
        default:
            return; // Non fare nulla se il numero non è valido
    }

    // Stampa la sprite scelta
    for (int i = 0; i < DIGIT_HEIGHT; i++) {
        mvwprintw(win, startY + i, startX, "%ls", digitSprite[i]);  // Usa "%ls" per wchar_t
    }
}

int digitsCount(int numero) {
    int cifre = 0;
    int temp = numero;
    
    do {
        cifre++;
        temp /= 10;
    } while (temp != 0);
    
    return cifre;
}

void digitsAnalyser(WINDOW *win, int score, int starty, int startx) {
    int temp = score;
    int digits = digitsCount(score);  // Usa il nome corretto
    int digitWidth = 6;  // Aggiungi la larghezza della sprite
    
    int digitArray[10];
    
    // Estrai le cifre
    for (int i = digits - 1; i >= 0; i--) {
        digitArray[i] = temp % 10;
        temp /= 10;
    }
    
    // Stampa dalla prima all'ultima
    for (int i = 0; i < digits; i++) {
        // Correggi l'ordine degli argomenti e aggiungi offset per ogni cifra
        printDigit(win, digitArray[i], startx + (i * digitWidth), starty);
    }
}

