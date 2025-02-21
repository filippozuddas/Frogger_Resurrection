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

    srand(time(NULL));
    
    int crocID = 1;          // Conta i coccodrilli totali
    int placedCrocCount = 0; // Quantità di coccodrilli effettivamente posizionati

    /* Inizializza la direzione (0 => destra; 1 => sinistra) e velocità dei flussi */
    flowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLOW; i++) flowDirection[i] = !flowDirection[i-1]; 
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
                // x casuale interna al limite (es. da 1 a GAME_WIDTH - CROC_LENGHT - 2)
                spawnX = rand() % (GAME_WIDTH - CROC_LENGHT) + 1;
                validPosition = isPositionValid(spawnX, spawnY,game->crocodile, placedCrocCount);
            }
            
            Crocodile *croc = &game->crocodile[crocID - 1]; 

            //inizializzo i valori prima della fork 
            croc->info.x = spawnX;
            croc->info.y = spawnY;
            croc->info.direction = flowDirection[flow]; 
            croc->info.speed = flowSpeed[flow]; 
            croc->info.ID = crocID;

            if (pipe(croc->mainToCrocPipe) < 0) {
                perror("Pipe creation error"); 
                exit(1); 
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
                srand(time(NULL) ^ getpid());

                close(croc->mainToCrocPipe[1]);
                close(game->pipeFd[0]); 

                moveCroc(croc, game->pipeFd);
                exit(0);
            }
            else {
                croc->info.pid = pid; 
                close(croc->mainToCrocPipe[0]);

                crocID++;
                placedCrocCount++;
            }
        }
    }
}

void moveCroc(Crocodile *croc, int *pipeFd) {
    int flags = fcntl(croc->mainToCrocPipe[0], F_GETFL, 0);
    fcntl(croc->mainToCrocPipe[0], F_SETFL, flags | O_NONBLOCK);
    
    while(1){
        Informations newInfo;
        if (read(croc->mainToCrocPipe[0], &newInfo, sizeof(Informations)) > 0) {
            croc->info.x = newInfo.x; 
            croc->info.y = newInfo.y; 
            croc->info.direction = newInfo.direction; 
            croc->info.speed = newInfo.speed; 
        }
        
        if (croc->info.direction == 0) {
            croc->info.x++; 
            if (croc->info.x >= GAME_WIDTH + 1 + CROC_LENGHT){
                //sleep(rand() % (5 - 4 + 1) + 4);
                croc->info.x = 0 - CROC_LENGHT;
            }
        }
        else {
            croc->info.x--; 
            if(croc->info.x < -1 -CROC_LENGHT) {
                //sleep(rand() % (5 - 4 + 1) + 4); 
                croc->info.x = GAME_WIDTH - 1 + CROC_LENGHT;
            }
        }
        
        write(pipeFd[1], &croc->info, sizeof(Informations));
        usleep(croc->info.speed * 10000);
    }
}

/*
 * Funzione per resettare le posizioni, direzione e velocità dei coccodrilli 
 * dopo la fine di ogni manche 
 */
void resetCroc(Game *game) {
    
    srand(time(NULL));
    
    int crocID = 1;          // Conta i coccodrilli totali
    int placedCrocCount = 0; // Quantità di coccodrilli effettivamente posizionati

    /* Inizializza la direzione (0 => destra; 1 => sinistra) e velocità dei flussi */
    flowDirection[0] = rand() % 2; 
    for (int i = 1; i < N_FLOW; i++) flowDirection[i] = !flowDirection[i-1]; 
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
                // x casuale interna al limite (es. da 1 a GAME_WIDTH - CROC_LENGHT - 2)
                spawnX = rand() % (GAME_WIDTH - CROC_LENGHT) + 1;
                validPosition = isPositionValid(spawnX, spawnY,game->crocodile, placedCrocCount);
            }
            
            Crocodile *croc = &game->crocodile[crocID - 1]; 

            croc->info.x = spawnX;
            croc->info.y = spawnY;
            croc->info.direction = flowDirection[flow]; 
            croc->info.speed = flowSpeed[flow]; 
            croc->info.ID = crocID;

            write(croc->mainToCrocPipe[1], &croc->info, sizeof(Informations));

            crocID++;
            placedCrocCount++; 
        }
    }
}

void killCroc(Game *game) {
    for (int i = 0; i < N_CROC; i++) {
        kill(game->crocodile[i].info.pid, SIGTERM); 
    }

    for (int i = 0; i < N_CROC; i++) {
        waitpid(game->crocodile[i].info.pid, NULL, 0); 
    }
}

void createProjectile(Crocodile *croc, int *pipeFd, Game *game, int projectileID) {
    Projectile projectile; 

    projectile.info.x = (croc->info.direction == 0) ? croc->info.x + CROC_LENGHT : croc->info.x; 
    projectile.info.y = croc->info.y + 2; 
    projectile.info.direction = croc->info.direction;
    projectile.info.speed = 3;
    projectile.info.ID = projectileID; 

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed"); 
        exit(1); 
    }
    else if (pid == 0) { 
        moveProjectile(&projectile, pipeFd); 
        exit(0);
    }
    else {
        projectile.info.pid = pid;

        int projectileIndex = -1;
        for(int i = 0; i < MAX_PROJECTILES; i++){
            if(game->projectiles[i].info.ID == -1){
                projectileIndex = i;
                break;
            }
        }
        game->projectiles[projectileIndex] = projectile;
        //fprintf(stderr, "[PADRE] Creato proiettile ID: %d, PID figlio: %d\n", projectileID, pid); // DEBUG: Stampa dopo la creazione
    }
}

void moveProjectile(Projectile *projectile, int *pipeFd) {
    while(1) {
        //fprintf(stderr, "[FIGLIO Proiettile] ID: %d, PID: %d\n", projectile.info.ID, getpid()); // DEBUG: Stampa ID e PID
        if (projectile->info.direction == 0) {
            projectile->info.x++;
            if (projectile->info.x > GAME_WIDTH) break;
        }
        else {
            projectile->info.x--;
            if (projectile->info.x < -1) break;
        }

        //fprintf(stderr, "[FIGLIO Proiettile] Invio: ID: %d, X: %d, Y: %d, PID: %d\n", projectile->info.ID, projectile->info.x, projectile->info.y, getpid()); // DEBUG
        write(pipeFd[1], &projectile->info, sizeof(Informations));
        usleep(projectile->info.speed * 10000);
    }
    //fprintf(stderr, "[FIGLIO Proiettile] Termino ID: %d, PID: %d\n", projectile->info.ID, getpid()); // DEBUG: Terminazione
    _exit(0);
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
                static int nextProjectileID = 47; 
                int projectileID = nextProjectileID++;


                createProjectile(&game->crocodile[selectedCroc], game->pipeFd, game, projectileID);
                lastShotTime = currentTime;
            }
        }
    }
}

void terminateProjectiles(Game *game) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (game->projectiles[i].info.ID != -1) {
            kill(game->projectiles[i].info.pid, SIGKILL);
            waitpid(game->projectiles[i].info.pid, NULL, 0); //  terminazione
            game->projectiles[i].info.ID = -1; // Libera lo slot
        }
    }
}