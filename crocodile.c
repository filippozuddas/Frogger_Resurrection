#include "crocodile.h"

int flowDirection[N_FLOW];
int flowSpeed[N_FLOW];

/*
 * isPositionValid controlla se la nuova x è sufficientemente distante
 * da altri coccodrilli “già posizionati” sulla stessa riga (usato dal processo padre).
 */
static int isPositionValid(int x_new, int y_new, Crocodile *crocodiles, int count) {
    for (int i = 0; i < count; i++) {
        // Se hanno la stessa riga
        if (crocodiles[i].info.y == y_new) {
            // Calcola distanza in base a x
            int diff = abs(x_new - crocodiles[i].info.x); 
            // Se si sovrappongono o sono troppo vicini
            if (diff < (CROC_LENGHT + MIN_CROC_DISTANCE)) {
                return 0; // posizione non valida
            }
        }
        
    }
    return 1; // posizione valida
}

void createCroc(Crocodile *croc, int *pipeFd) {

    int crocID = 1;          // Conta i coccodrilli totali
    int placedCrocCount = 0; // Quantità di coccodrilli effettivamente posizionati

    Crocodile tempCroc; 

    srand(time(NULL));

    //inizializza la direzione dei flussi  (0 => destra; 1 => sinistra)
    flowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLOW; i++) {
        flowDirection[i] = !flowDirection[i-1]; 
    }

    // Inizializza la velocità dei i flussi
    for (int i = 0; i < N_FLOW; i++) {
        flowSpeed[i] = (rand() % (MAX_V - MIN_V + 1)) + MIN_V;
    }

    // Creazione dei coccodrilli (N_FLOW flussi, CROC_PER_FLOW ciascuno)
    for (int flow = 0; flow < N_FLOW; flow++) {
        for (int j = 0; j < CROC_PER_FLOW; j++) {
            // Decidiamo la y in base al flusso corrente
            int spawnY = (LINES - 8) - (flow * CROC_HEIGHT);

            // Trova x casuale valida
            int spawnX = 0;
            int validPosition = 0;

            // Ciclo per trovare una posizione non sovrapposta
            while (!validPosition) {
                // x casuale interna al limite (es. da 1 a COLS - CROC_LENGHT - 2)
                spawnX = rand() % (COLS - CROC_LENGHT) + 1;
                validPosition = isPositionValid(spawnX, spawnY,croc, placedCrocCount);
            }

            //inizializzo i valori prima della fork 
            tempCroc.info.x = spawnX;
            tempCroc.info.y = spawnY;
            //tempCroc.isVisible = 1;
            tempCroc.info.direction = flowDirection[flow]; 
            tempCroc.info.speed = flowSpeed[flow]; 
            tempCroc.info.ID = crocID;

            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
                // Crocodile tempCroc;
                srand(time(NULL) ^ getpid());
                close(pipeFd[0]); // chiude il lato di lettura

                moveCroc(&tempCroc, pipeFd);
                exit(0);
            }
            else {
                tempCroc.info.pid = pid; 
                croc[crocID-1] = tempCroc; 

                crocID++;
                placedCrocCount++;
            }
        }
    }
}

void moveCroc(Crocodile *croc, int *pipeFd) {
    while(1){
        if (croc->info.direction == 0) {
            croc->info.x++; 
            if (croc->info.x >= COLS + 1 + CROC_LENGHT){
                sleep(rand() % (3 - 2 + 1) + 2);
                croc->info.x = 0 - CROC_LENGHT;
            }
        }
        else {
            croc->info.x--; 
            if(croc->info.x < -2 -CROC_LENGHT) {
                sleep(rand() % (3 - 2 + 1) + 2); 
                croc->info.x = COLS - 1 + CROC_LENGHT;
            }
        }

        // MessageType type = MSG_CROC;
        // write(pipeFd[1], &type, sizeof(type)); 
        write(pipeFd[1], &croc->info, sizeof(Informations));
        usleep(croc->info.speed * 10000);
    }
}