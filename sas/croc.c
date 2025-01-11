/*#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "map.h"

#define MAX_V 5
#define MIN_V 3
#define N_FLUSSI 8
#define CROC_PER_ROW 3
#define NUM_COCCODRILLI (N_FLUSSI * CROC_PER_ROW)
#define CROC_HEIGHT 4

//struttura per il un singolo coccodrillo 
typedef struct {
    int x;
    int y;
    int isVisible;
    pid_t pid;
} Coccodrillo;


//array globale per la velocità di ogni flusso
int rowSpeed[N_FLUSSI];

//array globale per la direzione di ogni flusso 
int rowDirection[N_FLUSSI]; 

const char *crocSprite[CROC_HEIGHT] = {
    "  __________   ",
    " / \\        \\",
    "| @ | ------ | ",
    " \\_/________/ "
};

//funzione per la gestione della logica e dei movimenti del coccodrillo 
void fun_coccodrillo(int pipe_scrittura, int croc_id) {
    Coccodrillo croc;
    
    int row = croc_id / CROC_PER_ROW;

    croc.y = (LINES - 6) - (croc_id * CROC_HEIGHT); 
    croc.x = (rowDirection[croc_id] == 0) ? 0 : COLS - 9; 
    croc.isVisible = 1; 
    croc.pid = getpid();

    srand(time(NULL) ^ getpid());
    sleep(rand() % MAX_V + MIN_V);

    //ciclo infinito per il movimento del coccodrillo 
    while (1) {
        //se il coccodrillo è all'interno dello schermo (visibile)
        if(croc.isVisible){
            if (rowDirection[croc_id] == 0){
                croc.x++; 
                //se il coccodrillo tocca il bordo destro viene reso invisibile
                if(croc.x >= COLS-9){
                    croc.isVisible = 0; 
                }
            }
            else {
                croc.x--; 
                //se il coccodrillo tocca il bordo sinistro
                if(croc.x <= 0){
                    croc.isVisible = 0;
                }
            }
        }
        
        //se il coccodrillo il coccodrillo non è più visibile all'interno dello schermo 
        else if (!croc.isVisible){
            sleep(rand() % MAX_V + MIN_V);
            //sleep(3 + rand() % (5 - 3 + 1)); 
            croc.x = (rowDirection[croc_id] == 0) ? 0 : COLS - 9;
            croc.isVisible = 1; 
        }

        write(pipe_scrittura, &croc, sizeof(Coccodrillo));
        usleep(rowSpeed[row] * 10000);
    }   
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    start_color();

    init_pair(4, COLOR_WHITE, COLOR_BLUE);
    bkgd(COLOR_PAIR(4));

    inizializza_mappa(); 
    disegna_mappa(); 

    WINDOW *crocWin = newwin(LINES, COLS, 0, 0); 

    //init_pair(4, COLOR_GREEN, COLOR_BLUE); 

    srand(time(NULL)); 
    //inizializzazione array per la velocità di ogni flusso 
    for (int i = 0; i < N_FLUSSI; i++) {
        rowSpeed[i] = (rand() % (8 - 5 + 1)) + 5; // Velocità casuale tra MIN_V e MAX_V
    }

    //inizializzazione array per la direzione di ogni flusso 
    rowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLUSSI; i++){
        rowDirection[i] = !rowDirection[i - 1]; 
    }

    int pipeFd[2];
    if (pipe(pipeFd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pids[NUM_COCCODRILLI];

    int k = 0;

    for (int i = 0; i < N_FLUSSI; i++) {
        for(int j = 0; j < CROC_PER_ROW; j++){
            if((pids[k] = fork()) == 0) {
                close(pipeFd[0]); 
                fun_coccodrillo(pipeFd[1], i); 
                exit(0); 
                k++;
            }
        }
    }

    close(pipeFd[1]);

    Coccodrillo croc[NUM_COCCODRILLI] = {0};

    while (1) {
        Coccodrillo tempCroc;
        int ch = getch();
        
        // Esci con 'q' o 'Q'
        if (ch == 'q' || ch == 'Q') {
            break;
        }

        if (read(pipeFd[0], &tempCroc, sizeof(Coccodrillo)) > 0) {
            // Trova l'indice corrispondente al pid o a un posto vuoto
            int index = -1;
            for (int i = 0; i < NUM_COCCODRILLI; i++) {
                if (croc[i].pid == tempCroc.pid || croc[i].pid == 0) {
                    index = i;
                    break;
                }
            }

            if (index != -1) {
                croc[index] = tempCroc;
            }
            
            wclear(crocWin); 

            //disegna_mappa(); 

            //attron(COLOR_PAIR(4));

            for (int i = 0; i < NUM_COCCODRILLI; i++) {
                if (croc[i].isVisible) {
                    for (int j = 0; j < CROC_HEIGHT; j++) {
                        mvprintw(croc[i].y + j, croc[i].x, "%s", crocSprite[j]);
                    }
                }
            }
            //attroff(COLOR_PAIR(4));
            
            // for(int i = 0; i < NUM_COCCODRILLI; i++)
            //     mvprintw(LINES-16+i, 1, "PID %d: %d", i, croc[i].pid);

            // for(int i = 0; i < NUM_COCCODRILLI; i++)
            //     mvprintw(LINES-16+i, 30, "PID %d: %d", i, pids[i]);

            wrefresh(crocWin);
        } 
    }

    // Termina tutti i processi figli
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        kill(pids[i], SIGTERM);
    }

    // Aspetta che tutti i processi figli terminino
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        waitpid(pids[i], NULL, 0);
    }

    endwin();
    return 0;
}


    "  __________   ",
    " / \\        \\",
    "| @ | ------ | ",
    " \\_/________/ "

*/


#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "map.h"

#define MAX_V 5
#define MIN_V 3
#define N_FLUSSI 8
#define CROC_PER_ROW 3
#define NUM_COCCODRILLI (N_FLUSSI * CROC_PER_ROW)
#define CROC_HEIGHT 4

// Struttura per un singolo coccodrillo 
typedef struct {
    int x;
    int y;
    int isVisible;
    pid_t pid;
} Coccodrillo;

// Array globale per la velocità di ogni flusso
int rowSpeed[N_FLUSSI];

// Array globale per la direzione di ogni flusso 
int rowDirection[N_FLUSSI]; 

const char *crocSprite[CROC_HEIGHT] = {
    "  __________   ",
    " / \\        \\",
    "| @ | ------ | ",
    " \\_/________/ "
};

// Funzione per la gestione della logica e dei movimenti del coccodrillo 
void fun_coccodrillo(int pipe_scrittura, int croc_id) {
    Coccodrillo croc;
    
    int row = croc_id; // CROC_PER_ROW;

    croc.y = (LINES - 6) - (row * CROC_HEIGHT); 
    croc.x = (rowDirection[row] == 0) ? 0 : COLS - 15; 
    croc.isVisible = 1; 
    croc.pid = getpid();

    srand(time(NULL) ^ getpid());
    sleep(rand() % MAX_V + MIN_V);

    // Ciclo infinito per il movimento del coccodrillo 
    while (1) {
        // Se il coccodrillo è all'interno dello schermo (visibile)
        if(croc.isVisible){
            if (rowDirection[row] == 0){
                croc.x++; 
                // Se il coccodrillo tocca il bordo destro viene reso invisibile
                if(croc.x >= COLS-15){
                    croc.isVisible = 0;
                }
            }
            else {
                croc.x--; 
                // Se il coccodrillo tocca il bordo sinistro
                if(croc.x <= 0){
                    croc.isVisible = 0;
                }
            }
        }

        else if(croc.isVisible == 0) {
            sleep(rand() % MAX_V + MIN_V);
            croc.x = (rowDirection[row] == 0) ? 0 : COLS - 15;
            croc.isVisible = 1; 
        }

        write(pipe_scrittura, &croc, sizeof(Coccodrillo));
        usleep(rowSpeed[row] * 10000);
    }   
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    start_color();

    //init_pair(4, COLOR_WHITE, COLOR_BLUE);
    //bkgd(COLOR_PAIR(4));

    //inizializza_mappa(); 
    //disegna_mappa(); 
    //refresh();

    srand(time(NULL)); 
    // Inizializzazione array per la velocità di ogni flusso 
    for (int i = 0; i < N_FLUSSI; i++) {
        rowSpeed[i] = (rand() % (8 - 5 + 1)) + 5; // Velocità casuale tra MIN_V e MAX_V
    }

    // Inizializzazione array per la direzione di ogni flusso 
    rowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLUSSI; i++){
        rowDirection[i] = !rowDirection[i - 1]; 
    }

    int pipeFd[2];
    if (pipe(pipeFd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pids[NUM_COCCODRILLI];

    int k = 0;

    for (int i = 0; i < N_FLUSSI; i++) {
        for(int j = 0; j < CROC_PER_ROW; j++){
            pids[k] = fork();  
            if(pids[k] == 0) {
                close(pipeFd[0]); 
                fun_coccodrillo(pipeFd[1], i); 
                exit(0); 
            }
            k++;
        }
    }

    close(pipeFd[1]);

    Coccodrillo croc[NUM_COCCODRILLI] = {0};

    // Creazione di una finestra separata per i coccodrilli
    //WINDOW *croc_win = newwin(LINES, COLS, 0, 0);

    while (1) {
        Coccodrillo tempCroc;
        int ch = getch();
        
        // Esci con 'q' o 'Q'
        if (ch == 'q' || ch == 'Q') {
            break;
        }

        if (read(pipeFd[0], &tempCroc, sizeof(Coccodrillo)) > 0) {
            // Trova l'indice corrispondente al pid o a un posto vuoto
            int index = -1;
            for (int i = 0; i < NUM_COCCODRILLI; i++) {
                if (croc[i].pid == tempCroc.pid || croc[i].pid == 0) {
                    index = i;
                    break;
                }
            }

            if (index != -1) {
                croc[index] = tempCroc;
            }

            // Cancella la finestra dei coccodrilli
            clear();

            // Disegna i coccodrilli nella finestra separata
            for (int i = 0; i < NUM_COCCODRILLI; i++) {
                if (croc[i].isVisible) {
                    for (int j = 0; j < CROC_HEIGHT; j++) {
                        mvprintw(croc[i].y + j, croc[i].x, "%s", crocSprite[j]);
                    }
                }
            }

            // Sovrapponi la finestra dei coccodrilli alla mappa
            //overlay(croc_win, stdscr);

            // Aggiorna la finestra dei coccodrilli
            refresh();
        } 
    }

    // Termina tutti i processi figli
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        kill(pids[i], SIGTERM);
    }

    // Aspetta che tutti i processi figli terminino
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        waitpid(pids[i], NULL, 0);
    }

    //delwin(croc_win); // Libera la memoria della finestra
    endwin();
    return 0;
}


