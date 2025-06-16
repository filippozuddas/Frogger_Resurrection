#include "crocodile.h"


int flowDirection[N_FLOW];
int flowSpeed[N_FLOW];
int MIN_V;
int MAX_V;

static pthread_t crocThreads[N_CROC];

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

void createCroc(Game *game) {
    switch (game->difficulty) {
        case 2: MIN_V = 3; MAX_V = 5; break;
        case 1: MIN_V = 6; MAX_V = 8; break;
        case 0: MIN_V = 9; MAX_V = 11; break;
        default: MIN_V = 9; MAX_V = 11; break;
    }

    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    int crocID = 1;
    int placedCrocCount = 0;

    // Inizializza la direzione (0 => destra; 1 => sinistra) e velocitÃ  dei flussi
    flowDirection[0] = rand() % 2;
    for (int i = 1; i < N_FLOW; i++) {
        flowDirection[i] = !flowDirection[i - 1];
    }
    for (int i = 0; i < N_FLOW; i++) {
        flowSpeed[i] = (rand() % (MAX_V - MIN_V + 1)) + MIN_V;
    }

    // Creazione dei coccodrilli (N_FLOW flussi, CROC_PER_FLOW ciascuno)
    for (int flow = 0; flow < N_FLOW; flow++) {
        for (int j = 0; j < CROC_PER_FLOW; j++) {
            int spawnY = (GAME_HEIGHT - 9) - (flow * CROC_HEIGHT);
            int spawnX = 0;
            int attempts = 0; // Contatore di tentativi

            int validPosition = 0;
            while (!validPosition) {
                // x casuale interna al limite (es. da 1 a GAME_WIDTH - CROC_LENGHT - 2)
                spawnX = rand() % (GAME_WIDTH - CROC_LENGHT) + 1;
                validPosition = isPositionValid(spawnX, spawnY, game->crocodile, placedCrocCount);
            }


            if (validPosition) {
                // Inizializzazione delle informazioni del coccodrillo PRIMA di creare il thread
                game->crocodile[crocID - 1].info.x = spawnX;
                game->crocodile[crocID - 1].info.y = spawnY;
                game->crocodile[crocID - 1].info.direction = flowDirection[flow];
                game->crocodile[crocID - 1].info.speed = flowSpeed[flow];
                game->crocodile[crocID - 1].info.ID = crocID;
                game->crocodile[crocID - 1].info.active = 1; // Imposta il coccodrillo come attivo
                //game->crocodile[crocID - 1].projectilesRemaining = 3; // Initialize grenadesRemaining

                // Creazione del thread DOPO aver inizializzato le info
                if (pthread_create(&game->crocodile[crocID - 1].thread, NULL, moveCroc, (void *)&game->crocodile[crocID - 1]) != 0) {
                    perror("Failed to create crocodile thread");
                    exit(EXIT_FAILURE);
                }
                

                crocID++;
                placedCrocCount++;
            }
        }
    }
}
    
    
void* moveCroc(void* arg) {
    Crocodile *croc = (Crocodile *)arg;
    
    while (1) {
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
        
        writeMain(croc->info);
        usleep((MAX_V + MIN_V - croc->info.speed) * 10000); // Movimento
    }

    pthread_exit(NULL);
}


void resetCroc(Game *game) {
    // Determine speed range based on difficulty
    switch (game->difficulty) {
        case 2: MIN_V = 3; MAX_V = 5; break;
        case 1: MIN_V = 6; MAX_V = 8; break;
        case 0: MIN_V = 9; MAX_V = 11; break;
        default: MIN_V = 9; MAX_V = 11; break;
    }

    srand(time(NULL)); // Seed only once per game (moved to initGame)
    int crocID = 1;
    int placedCrocCount = 0;

    // Initialize flow directions and speeds
    flowDirection[0] = rand() % 2;
    for (int i = 1; i < N_FLOW; i++) {
        flowDirection[i] = !flowDirection[i - 1];
    }
    for (int i = 0; i < N_FLOW; i++) {
        flowSpeed[i] = (rand() % (MAX_V - MIN_V + 1)) + MIN_V;
    }

    // Create crocodiles for each flow
    for (int flow = 0; flow < N_FLOW; flow++) {
        for (int j = 0; j < CROC_PER_FLOW; j++) {
            int spawnY = (GAME_HEIGHT - 9) - (flow * CROC_HEIGHT);
            int spawnX;
            int attempts = 0; // Contatore di tentativi

            bool validPosition = false;

            // Ciclo di posizionamento con limite di tentativi e gestione degli errori
            do {
                spawnX = rand() % (GAME_WIDTH - CROC_LENGHT) + 1;
                validPosition = isPositionValid(spawnX, spawnY, game->crocodile, placedCrocCount);
                attempts++;

                if (attempts > 100) { // Limite di tentativi aumentato
                    fprintf(stderr, "Impossibile trovare una posizione valida per il coccodrillo %d\n", crocID);
                    // Invece di uscire, riduci il numero di coccodrilli per questo flusso
                    break; 
                }
            } while (!validPosition);

            if (validPosition) {
                
                // Alloca dinamicamente la memoria per il coccodrillo
                Crocodile *croc = (Crocodile *)malloc(sizeof(Crocodile));

                if (croc == NULL) {
                    perror("Failed to allocate memory for crocodile");
                    exit(EXIT_FAILURE);
                }

                printf("Croc %d: x = %d, y = %d\n", crocID, croc->info.x, croc->info.y);

            // Initialize crocodile information
            croc->info.x = spawnX;
            croc->info.y = spawnY;
            croc->info.direction = flowDirection[flow];
            croc->info.speed = flowSpeed[flow];
            croc->info.ID = crocID;
            croc->info.active = 1;
            //croc->projectilesRemaining = 3; // Initialize grenadesRemaining

            writeProd(game->crocodile[j].info);
        }

        crocID++;
        placedCrocCount++;
    }
    }
}



void killCroc(Game *game) {
    // Cancel and join crocodile threads
    for (int i = 0; i < N_CROC; i++) {
        if (game->crocodile[i].info.active) { // Only cancel active threads
            pthread_cancel(game->crocodile[i].thread);
            pthread_join(game->crocodile[i].thread, NULL);
            game->crocodile[i].info.active = 0; // Mark as inactive
        }
    }
}

void createProjectile(Game *game, Crocodile *croc, int projectileID, int projectileIndex) {

    //pthread_mutex_lock(&projectile_mutex); // LOCK - Protect projectile array

    Projectile *projectile = &game->projectiles[projectileIndex];

    projectile->info.x = (croc->info.direction == 0) ? croc->info.x + CROC_LENGHT : croc->info.x;
    projectile->info.y = croc->info.y + 2; // +1 o -1?  Depends on your coordinate system
    projectile->info.direction = croc->info.direction;
    projectile->info.speed = 2;
    projectile->info.ID = projectileID;  // Unique ID (important!)
    projectile->info.active = 1;
    //pthread_mutex_unlock(&projectile_mutex); // UNLOCK

    pthread_create(&projectile->thread, NULL, moveProjectile, (void *)projectile);
}

void* moveProjectile(void* arg) {
    Projectile* projectile = (Projectile*)arg;

    //pthread_mutex_lock(&projectile_mutex);
    while (projectile->info.active) {
        if (projectile->info.direction == 0) {
            projectile->info.x++;
            if (projectile->info.x > GAME_WIDTH) {
                 break; // Exit loop if off-screen
            }
        } else {
            projectile->info.x--;
            if (projectile->info.x < -1) {
                break; // Exit loop if off-screen
            }
        }

        // Write projectile info to the MAIN buffer
        writeMain(projectile->info);
        usleep(projectile->info.speed * 10000); // Adjust speed as needed
    }
    projectile->info.active = 0; // Ensure it's marked as inactive on exit
    projectile->info.ID = -1; 

    //pthread_mutex_lock(&projectile_mutex);
    pthread_exit(NULL);
}

void handleProjectileGeneration(Game *game) {
    static time_t lastShotTime = 0;
    time_t currentTime = time(NULL);

    if ((rand() % 100 == 1) && (currentTime - lastShotTime >= 2)) {
        int visibleCrocs[N_CROC];
        int visibleCount = 0;

        for (int i = 0; i < N_CROC; i++) {
            if (game->crocodile[i].info.x >= -CROC_LENGHT &&
                game->crocodile[i].info.x <= GAME_WIDTH - CROC_LENGHT) {
                visibleCrocs[visibleCount++] = i;
            }
        }

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

            // Se abbiamo trovato uno slot libero...
            if (projectileIndex != -1) {
                // Genera un ID univoco (usa un timestamp o un contatore globale)
                static int nextProjectileID = 57; 
                int projectileID = nextProjectileID++;


                createProjectile(game, &game->crocodile[selectedCroc], projectileID, projectileIndex);
                lastShotTime = currentTime;
            }
        }
    }
}

void terminateProjectiles(Game *game) {

    //pthread_mutex_lock(&projectile_mutex); // UNLOCK
    
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (game->projectiles[i].info.active) {
            pthread_cancel(game->projectiles[i].thread); // Cancel the thread
            pthread_join(game->projectiles[i].thread, NULL);   // Wait for it to finish
            game->projectiles[i].info.active = 0;       // Mark as inactive
            game->projectiles[i].info.ID = -1;         // Reset ID
        }
    }
    //pthread_mutex_unlock(&projectile_mutex); // UNLOCK
}