/*#include "crocodile.h"
#include <stdlib.h>


int flowDirection[N_FLOW]; 
int flowSpeed[N_FLOW]; 

const char *crocSprite[CROC_HEIGHT] = {
    "  __________   ",
    " / \\        \\",
    "| @ | ------ | ",
    " \\_/________/ "
};

void createCroc(int pipeFd[]) {
    //inizializzazione randomica della direzione di ogni flusso 
    flowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLOW; i++){
        flowDirection[i] = !flowDirection[i - 1];
    }

    //inizializzazione della velocità di ogni flusso 
    for (int i = 0; i < N_FLOW; i++) {
        flowSpeed[i] = (rand() % (MAX_V - MIN_V + 1)) + MAX_V; 
    }

    pid_t pids[N_CROC]; 
    int crocID = 0; 

    for (int i = 0; i < N_FLOW; i++) {
        for (int j = 0; j < CROC_PER_FLOW; j++) {
            //pids[crocID] = fork(); 
            pid_t pid = fork();
            if (pid < 0){
                perror("Fork error"); 
                exit(1); 
            }
            else if (pid == 0) {
                Crocodile tempCroc; 

                srand(time(NULL) ^ getpid()); 
                tempCroc.x = (flowDirection[i] == 0) ? 0 : COLS - 15; 
                tempCroc.y = (LINES - 6) - (i * CROC_HEIGHT); 
                tempCroc.isVisible = 1;
                tempCroc.pid = getpid(); 

                sleep(rand() % (MAX_V - MIN_V + 1) + MIN_V);

                close(pipeFd[0]); 

                while(1){
                    moveCroc(&tempCroc, i); 
                    write(pipeFd[1], &tempCroc, sizeof(Crocodile)); 
                    usleep(flowSpeed[i] * 10000); 
                }
                exit(0); 
            }  
            else 
                pids[crocID++] = pid;
        }
    }
}

void moveCroc(Crocodile *croc, int flow) {
    if (croc->isVisible) {
        if(flowDirection[flow] == 0) {
            croc->x++; 
            
            if (croc->x >= COLS - 15) {
                croc->isVisible = 0; 
            }
        }
        else{
            croc->x--; 
            
            if (croc->x <= 0) {
                croc->isVisible = 0;
            }
        }
    }
    else {
        sleep(rand() % (MAX_V - MIN_V + 1) + MIN_V);
        croc->x = (flowDirection[flow] == 0) ? 0 : COLS - 15;
        croc->isVisible = 1; 
    }
}
*/
#include "crocodile.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

// Definisci una distanza minima di sicurezza tra coccodrilli (per evitare sovrapposizioni)
#define MIN_CROC_DISTANCE 8

int flowDirection[N_FLOW];
int flowSpeed[N_FLOW];

const char *crocSprite[CROC_HEIGHT] = {
    "  __________   ",
    " / \\        \\",
    "| @ | ------ | ",
    " \\_/________/ "
};

/*
 * isPositionValid controlla se la nuova x è sufficientemente distante
 * da altri coccodrilli “già posizionati” sulla stessa riga (usato dal processo padre).
 */
static int isPositionValid(int x_new, int y_new, Crocodile *crocodiles, int count) {
    for (int i = 0; i < count; i++) {
        if (crocodiles[i].isVisible) {
            // Se hanno la stessa riga
            if (crocodiles[i].y == y_new) {
                // Calcola distanza in base a x
                int diff = abs(x_new - crocodiles[i].x); 
                // Se si sovrappongono o sono troppo vicini
                if (diff < (CROC_LENGHT + MIN_CROC_DISTANCE)) {
                    return 0; // posizione non valida
                }
            }
        }
    }
    return 1; // posizione valida
}

void createCroc(int pipeFd[]) {
    pid_t pids[N_CROC];
    // Array temporaneo di coccodrilli solo per il controllo dello spawn
    Crocodile crocodilesSpawned[N_CROC];
    int crocID = 0;          // Conta i coccodrilli totali
    int placedCrocCount = 0; // Quantità di coccodrilli effettivamente posizionati

    srand(time(NULL));

    // Inizializza la direzione e la velocità per i flussi
    for (int i = 0; i < N_FLOW; i++) {
        // 0 => destra; 1 => sinistra
        flowDirection[i] = rand() % 2;
        flowSpeed[i] = (rand() % (MAX_V - MIN_V + 1)) + MIN_V;
    }

    // Creazione dei coccodrilli (N_FLOW flussi, CROC_PER_FLOW ciascuno)
    for (int flow = 0; flow < N_FLOW; flow++) {
        for (int j = 0; j < CROC_PER_FLOW; j++) {
            // Decidiamo la y in base al flusso corrente
            int spawnY = (LINES - 6) - (flow * CROC_HEIGHT);

            // Trova x casuale valida
            int validPositionFound = 0;
            int spawnX = 0;

            // Ciclo per trovare una posizione non sovrapposta
            while (!validPositionFound) {
                // x casuale interna al limite (es. da 1 a COLS - CROC_LENGHT - 2)
                spawnX = rand() % (COLS - CROC_LENGHT - 2) + 1;
                validPositionFound = isPositionValid(spawnX, spawnY,
                                                     crocodilesSpawned,
                                                     placedCrocCount);
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
                // Processo figlio
                Crocodile tempCroc;
                srand(time(NULL) ^ getpid());

                tempCroc.x = spawnX;
                tempCroc.y = spawnY;
                tempCroc.isVisible = 1;
                tempCroc.pid = getpid();

                close(pipeFd[0]); // chiude il lato di lettura

                // Pausa iniziale per desincronizzare un po' i coccodrilli
                //sleep(rand() % (MAX_V - MIN_V + 1) + MIN_V);

                // Loop infinito per muovere il coccodrillo e inviare i dati
                while (1) {
                    moveCroc(&tempCroc, flow);
                    write(pipeFd[1], &tempCroc, sizeof(Crocodile));
                    usleep(flowSpeed[flow] * 10000);
                }
                exit(EXIT_SUCCESS);
            }
            else {
                // Processo padre: salva PID e posizione in crocodilesSpawned
                pids[crocID] = pid;

                crocodilesSpawned[crocID].x = spawnX;
                crocodilesSpawned[crocID].y = spawnY;
                crocodilesSpawned[crocID].isVisible = 1;
                crocodilesSpawned[crocID].pid = pid;

                crocID++;
                placedCrocCount++;
            }
        }
    }
}

void moveCroc(Crocodile *croc, int flow) {
    if (croc->isVisible) {
        // Direzione 0 => muovi a destra
        // Direzione 1 => muovi a sinistra
        if (flowDirection[flow] == 0) {
            croc->x++;
            // Se superi il margine a destra
            if (croc->x >= (COLS - CROC_LENGHT)) {
                croc->isVisible = 0;
            }
        }
        else {
            croc->x--;
            if (croc->x <= 0) {
                croc->isVisible = 0;
            }
        }
    }
    else {
        /*
        Se non è visibile, aspetta un po' e ricollocalo
        (per simulare la “ricomparsa” del coccodrillo).
        */
        sleep(rand() % (3 - 1 + 1) + 1);
        if (flowDirection[flow] == 0) {
            // Se il flow è verso destra, ricomincia da sinistra
            croc->x = 0;
        }
        else {
            // Se il flow è verso sinistra, ricomincia da destra
            croc->x = COLS - CROC_LENGHT;
        }
        croc->isVisible = 1;
    }
}