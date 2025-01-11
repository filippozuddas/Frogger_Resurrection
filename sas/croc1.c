#include "croc.h"

void initSpeed() {
    srand(time(NULL)); 
    //inizializzazione array per la velocità di ogni flusso 
    for (int i = 0; i < N_FLUSSI; i++) {
        rowSpeed[i] = (rand() % (8 - 5 + 1)) + 5; // Velocità casuale tra MIN_V e MAX_V
    }
}

void initDirection() {
    //inizializzazione array per la direzione di ogni flusso 
    rowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLUSSI; i++){
        rowDirection[i] = !rowDirection[i - 1]; 
    }
}

void createCroc(int pipeFd[2]) {
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
}

void printCroc(char *crocSprite, Coccodrillo *croc) {
    clear(); 

            for (int i = 0; i < NUM_COCCODRILLI; i++) {
                if (croc[i].isVisible) {
                    for (int j = 0; j < CROC_HEIGHT; j++) {
                        mvprintw(croc[i].y + j, croc[i].x, "%s", crocSprite[j]);
                    }
                }
            }
    refresh();
}