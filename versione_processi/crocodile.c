#include "crocodile.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int flowDirection[N_FLOW];
int flowSpeed[N_FLOW];
int MIN_V;  
int MAX_V; 

/*
 * isPositionValid controlla se la nuova x è sufficientemente distante
 * da altri coccodrilli “già posizionati” sulla stessa riga (usato dal processo padre).
 */
static int isPositionValid(int x_new, int y_new, Crocodile *crocodiles, int count) {
    for (int i = 0; i < count; i++) {
        // Se hanno la stessa riga
        if (crocodiles[i].info.y == y_new) {
            // Calcola distanza in base a x
            int diff = abs(crocodiles[i].info.x - x_new); 
            // Se si sovrappongono o sono troppo vicini
            if (diff < (CROC_LENGHT + MIN_CROC_DISTANCE)) {
                return 0; // posizione non valida
            }
        }
    }
    return 1; // posizione valida
}

// createCroc crea i processi coccodrillo e li inizializza con le informazioni necessarie.
void createCroc(Game *game) {
    
    // Setting della velocità in base alla difficoltà scelta
    switch (game->difficulty) {
        case 2:
            MIN_V = 3;
            MAX_V = 5;
            break;
        case 1:
            MIN_V = 6;
            MAX_V = 8;
            break;
        case 0:
            MIN_V = 9;
            MAX_V = 11;
            break;
        default:
            MIN_V = 9;
            MAX_V = 11;
            break;
    }

    srand(time(NULL)); 

    int crocID = 1;
    int placedCrocCount = 0; 

    /* Inizializza la direzione (0 => destra; 1 => sinistra) e velocità dei flussi */
    flowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLOW; i++) flowDirection[i] = !flowDirection[i - 1]; 
    for (int i = 0; i < N_FLOW; i++) flowSpeed[i] = (rand() % (MAX_V - MIN_V + 1)) + MIN_V;

    // Creazione dei coccodrilli (N_FLOW flussi, CROC_PER_FLOW ciascuno)
    for (int flow = 0; flow < N_FLOW; flow++) {
        for (int j = 0; j < CROC_PER_FLOW; j++) {
            // Decidiamo la y in base al flusso corrente
            int spawnY = (GAME_HEIGHT - 9) - (flow * CROC_HEIGHT);

            // Trova x casuale valida
            int spawnX = 0;
            int validPosition = 0;

            // Ciclo per trovare una posizione non sovrapposta
            while (!validPosition) {
                // x casuale interna al limite della mappa di gioco 
                spawnX = rand() % (GAME_WIDTH - CROC_LENGHT) + 1;
                validPosition = isPositionValid(spawnX, spawnY, game->crocodile, placedCrocCount);
            }

            // Controlla che crocID non superi il numero massimo di coccodrilli
            if (crocID - 1 >= N_CROC) {
                exit(EXIT_FAILURE);
            }

            // Puntatore al coccodrillo corrente 
            Crocodile *croc = &game->crocodile[crocID - 1]; 

            // Inizializzo i valori prima della fork 
            croc->info.x = spawnX;
            croc->info.y = spawnY;
            croc->info.direction = flowDirection[flow]; 
            croc->info.speed = flowSpeed[flow]; 
            croc->info.ID = crocID;

            // Creazione della pipe per il coccodrillo, utilizzata per il reset alla termine di una manche 
            if (pipe(croc->mainToCrocPipe) < 0) {
                perror("Pipe creation error"); 
                exit(EXIT_FAILURE); 
            }

            // creazione del processo coccodrillo 
            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            } else if (pid == 0) { // Processo figlio
                srand(time(NULL) ^ getpid());

                close(croc->mainToCrocPipe[1]); // Chiudi il lato di scrittura della pipe
                close(game->pipeFd[0]); // Chiudi il lato di lettura della pipe principale

                // funzione per muovere il coccodrillo
                moveCroc(croc, game->pipeFd);
                _exit(0); // Termina il processo figlio
            } else { // Processo padre
                croc->info.pid = pid;   // Salvo il pid del processo coccodrillo
                close(croc->mainToCrocPipe[0]); // Chiudi il lato di lettura della pipe

                crocID++;
                placedCrocCount++;
            }
        }
    }
}

void moveCroc(Crocodile *croc, int *pipeFd) {

    // setting della lettura non bloccante della pipe inversa
    int flags = fcntl(croc->mainToCrocPipe[0], F_GETFL, 0);
    fcntl(croc->mainToCrocPipe[0], F_SETFL, flags | O_NONBLOCK);

    // ciclo infinito per il movimento del coccodrillo
    while (1) {
        // lettura delle informazioni dalla pipe inversa per un eventuale reset del coccodrillo
        Informations newInfo;
        if (readData(croc->mainToCrocPipe[0], &newInfo, sizeof(Informations)) > 0) {
            croc->info.x = newInfo.x; 
            croc->info.y = newInfo.y; 
            croc->info.direction = newInfo.direction; 
            croc->info.speed = newInfo.speed; 
        }

        // logica di movimento del coccodrillo
        if (croc->info.direction == 0) {
            croc->info.x++; 
            if (croc->info.x >= GAME_WIDTH + 1 + CROC_LENGHT) {
                croc->info.x = 0 - CROC_LENGHT;
            }
        } else {
            croc->info.x--; 
            if (croc->info.x < -1 - CROC_LENGHT) {
                croc->info.x = GAME_WIDTH - 1 + CROC_LENGHT;
            }
        }

        // Scrittura delle informazioni aggiornate nella pipe principale
        if (writeData(pipeFd[1], &croc->info, sizeof(Informations)) == -1) {
            perror("Errore nella scrittura sulla pipe");
            exit(EXIT_FAILURE);
        }

        // gestione della velocità del coccodrillo
        usleep((MAX_V + MIN_V - croc->info.speed) * 10000);
    }
}

/* 
 * funzione per resettare i coccodrilli alla fine di una manche, senza terminare i processi
 * essenzialmente identica alla createCroc, ma senza fork
 */
void resetCroc(Game *game) {

    switch (game->difficulty) {
        case 2:
            MIN_V = 3;
            MAX_V = 5;
            break;
        case 1:
            MIN_V = 6;
            MAX_V = 8;
            break;
        case 0:
            MIN_V = 9;
            MAX_V = 11;
            break;
        default:
            MIN_V = 9;
            MAX_V = 11;
            break;
    }

    srand(time(NULL));

    int crocID = 1;          
    int placedCrocCount = 0; 

    flowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLOW; i++) flowDirection[i] = !flowDirection[i - 1]; 
    for (int i = 0; i < N_FLOW; i++) flowSpeed[i] = (rand() % (MAX_V - MIN_V + 1)) + MIN_V;

    for (int flow = 0; flow < N_FLOW; flow++) {
        for (int j = 0; j < CROC_PER_FLOW; j++) {
            int spawnY = (GAME_HEIGHT - 9) - (flow * CROC_HEIGHT);

            int spawnX = 0;
            int validPosition = 0;

            while (!validPosition) {
                spawnX = rand() % (GAME_WIDTH - CROC_LENGHT) + 1;
                validPosition = isPositionValid(spawnX, spawnY, game->crocodile, placedCrocCount);
            }

            if (crocID - 1 >= N_CROC) {
                fprintf(stderr, "Errore: crocID supera il numero massimo di coccodrilli\n");
                exit(EXIT_FAILURE);
            }

            Crocodile *croc = &game->crocodile[crocID - 1]; 

            croc->info.x = spawnX;
            croc->info.y = spawnY;
            croc->info.direction = flowDirection[flow]; 
            croc->info.speed = flowSpeed[flow]; 
            croc->info.ID = crocID;

            if (writeData(croc->mainToCrocPipe[1], &croc->info, sizeof(Informations)) == -1) {
                perror("Errore nella scrittura sulla pipe");
                exit(EXIT_FAILURE);
            }

            crocID++;
            placedCrocCount++; 
        }
    }
}

// killCroc termina tutti i processi coccodrillo e attende la loro terminazione
void killCroc(Game *game) {
    for (int i = 0; i < N_CROC; i++) {
        kill(game->crocodile[i].info.pid, SIGKILL); 
        waitpid(game->crocodile[i].info.pid, NULL, 0); 
    }
}

//  createProjectile crea un nuovo proiettile associato a un coccodrillo.
void createProjectile(Crocodile *croc, int *pipeFd, Game *game, int projectileID) {
    // inizializzazione dei valori del proiettile
    Projectile projectile; 

    projectile.info.x = (croc->info.direction == 0) ? croc->info.x + CROC_LENGHT : croc->info.x; 
    projectile.info.y = croc->info.y + 2; 
    projectile.info.direction = croc->info.direction;
    projectile.info.speed = 2;
    projectile.info.ID = projectileID; 

    // crezione del processo proiettile
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed"); 
        exit(1); 
    } else if (pid == 0) { 
        moveProjectile(&projectile, pipeFd); 
        exit(0);
    }
    else {
        // salvo il pid del processo proiettile
        projectile.info.pid = pid;


        /*
         * Scansiona l'array `game->projectiles` per trovare un elemento il cui campo ID sia -1,
         * che indica uno slot non utilizzato. Una volta trovato il primo slot libero, memorizza
         * il suo indice, interrompe la ricerca e assegna il nuovo proiettile a quella posizione.
         */
        int projectileIndex = -1;
        for(int i = 0; i < MAX_PROJECTILES; i++){
            if(game->projectiles[i].info.ID == -1){
                projectileIndex = i;
                break;
            }
        }
        game->projectiles[projectileIndex] = projectile;
    }
}

// moveProjectile gestisce il movimento del proiettile, aggiornando la sua posizione e inviando le informazioni tramite pipe
void moveProjectile(Projectile *projectile, int *pipeFd) {
    while(1) {
        if (projectile->info.direction == 0) {
            projectile->info.x++;
            if (projectile->info.x > GAME_WIDTH) break;
        }
        else {
            projectile->info.x--;
            if (projectile->info.x < -3) break;
        }

        writeData(pipeFd[1], &projectile->info, sizeof(Informations));
        usleep(projectile->info.speed * 10000);
    }
    _exit(0);
}

/* 
 * handleProjectileGeneration gestisce la generazione dei proiettili in modo casuale
 * basandosi sulla visibilità dei coccodrilli e sul tempo trascorso dall'ultimo tiro.
 */
void handleProjectileGeneration(Game *game) {
    // creazione di una variabile time_t per tenere traccia dell'ultimo tiro
    static time_t lastShotTime = 0;
    time_t currentTime = time(NULL);

    // Se il tempo trascorso dall'ultimo tiro è maggiore o uguale a 2 secondi e un numero casuale è 1 (probabilità dell'1%)
    if ((rand() % 100 == 1) && (currentTime - lastShotTime >= 2)) {
        // Trova i coccodrilli visibili
        int visibleCrocs[N_CROC];
        int visibleCount = 0;

        for (int i = 0; i < N_CROC; i++) {
            if (game->crocodile[i].info.x >= -CROC_LENGHT &&
                game->crocodile[i].info.x <= GAME_WIDTH - CROC_LENGHT) {
                visibleCrocs[visibleCount++] = i;
            }
        }

        // Se ci sono coccodrilli visibili, seleziona uno di loro casualmente
        if (visibleCount > 0) {
            int selectedCroc = visibleCrocs[rand() % visibleCount];

            // Trova il primo slot libero in game->projectiles
            int projectileIndex = -1;
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (game->projectiles[i].info.ID == -1) {
                    projectileIndex = i;
                    break;
                }
            }

            // Se abbiamo trovato uno slot libero
            if (projectileIndex != -1) {
                // Genera un ID univoco
                static int nextProjectileID = 57; 
                int projectileID = nextProjectileID++;

                // Crea il proiettile associato al coccodrillo selezionato
                createProjectile(&game->crocodile[selectedCroc], game->pipeFd, game, projectileID);
                lastShotTime = currentTime;
            }
        }
    }
}

// terminateProjectiles termina tutti i processi proiettile e libera gli slot nell'array
void terminateProjectiles(Game *game) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (game->projectiles[i].info.ID != -1) {
            kill(game->projectiles[i].info.pid, SIGKILL);
            waitpid(game->projectiles[i].info.pid, NULL, 0); //  terminazione
            game->projectiles[i].info.ID = -1; // Libera lo slot
        }
    }
}