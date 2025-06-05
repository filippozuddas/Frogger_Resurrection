#include "frog.h"


void createFrog(Game *game) {

    int result;

    //Frog *frog = &game->frog;
    game->frog.info.x = ((GAME_WIDTH - 1) / 2) - 4;
    game->frog.info.y = GAME_HEIGHT - 5;
    game->frog.info.ID = 0;
    game->frog.grenadesRemaining = 5;
    game->frog.lives = 3;
    game->frog.score = 0;
    game->frog.isOnCroc = 0;

    
    //pthread_create(&game->frog.thread, NULL, inputHandler, (void*)game);
    if (pthread_create(&game->frog.thread, NULL, (void *)inputHandler, (void *)game) != 0) {
        perror("Failed to create frog thread");
        exit(EXIT_FAILURE);
   }

    
}

void* inputHandler(void* params) {

    Game *game = (Game*)params;
    Informations frogInfo;
    int c;


    // creo struttura per salvare da lettura

    while (game->isRunning) { // Leggi dalla finestra corretta
        
        pthread_mutex_lock(&ncurses_mutex);
        c = wgetch(game->gameWin);
        pthread_mutex_unlock(&ncurses_mutex);

        // frogInfo = readProd(); // legge le informazioni dal buffer condiviso con il consumatore

        // if (frogInfo.ID == 0) {
        //     game->frog.info.x = frogInfo.x;
        // }

        if(c != ERR){
            //pthread_mutex_lock(&buffer_mutex);
            switch (c) {
                case 'w':
                case 'W':
                case KEY_UP:
                    game->frog.info.y = (game->frog.info.y > 0) ? game->frog.info.y - FROG_HEIGHT : game->frog.info.y;
                    break;
                case 's':
                case 'S':
                case KEY_DOWN:
                    game->frog.info.y = (game->frog.info.y < GAME_HEIGHT - FROG_HEIGHT - 1) ? game->frog.info.y + FROG_HEIGHT : game->frog.info.y;
                    break;
                case 'd':
                case 'D':
                case KEY_RIGHT:
                    game->frog.info.x = (game->frog.info.x < GAME_WIDTH - FROG_WIDTH) ? game->frog.info.x + 1 : game->frog.info.x;
                    break;
                case 'a':
                case 'A':
                case KEY_LEFT:
                    game->frog.info.x = (game->frog.info.x > 0) ? game->frog.info.x - 1 : game->frog.info.x;
                    break;
               // case ' ': // Spacebar for grenade
               //     if (game->frog.grenadesRemaining > 0) {
               //         Informations grenadeSignal;
               //         grenadeSignal.ID = -1; // -1 per granata a destra
               //         writeMain(grenadeSignal);
//
               //         grenadeSignal.ID = -2; // -2 per granata a sinistra
               //         writeMain(grenadeSignal);
//
               //         game->frog.grenadesRemaining--;
               // }
                break;
                case 'q': //Add exit condition
                case 'Q':
                    game->isRunning = 0;
                    pthread_exit(NULL);
                    break;
            }

            //pthread_mutex_lock(&buffer_mutex);
            writeMain(game->frog.info);
            //pthread_mutex_unlock(&buffer_mutex);
            //usleep(1000);
    
        }

        // readProd(frogInfo); 
        // pthread_mutex_lock(&buffer2_mutex);
        // game->frog.info = frogInfo;
        // pthread_mutex_unlock(&buffer2_mutex);
    }
    pthread_exit(NULL);
}
    
void createGrenade(Game *game, int direction){

    Grenade* grenade = malloc(sizeof(Grenade));

    int grenadeID, grenadeIndex = findFreeGrenadeSlot(game);

    grenadeID++;

    if (grenadeIndex >= 0) {
        
        pthread_mutex_lock(&granades_mutex);
        game->grenades[grenadeIndex].info.x = (direction == 1) ? game->frog.info.x + FROG_WIDTH : game->frog.info.x - 1;
        game->grenades[grenadeIndex].info.y = game->frog.info.y;
        game->grenades[grenadeIndex].info.direction = direction;
        game->grenades[grenadeIndex].info.speed = 3; // Example speed
        game->grenades[grenadeIndex].info.ID = grenadeID; // Use provided ID
        game->grenades[grenadeIndex].info.active = 1;      // Mark as active
        
        pthread_mutex_unlock(&granades_mutex);
        pthread_create(&game->grenades[grenadeIndex].thread, NULL, (void  *)moveGrenade, (void *)game->grenades); 
    }
}

void moveGrenade(void * params) {
    Grenade *grenade = (Grenade *)params;

    pthread_mutex_lock(&granades_mutex);
    while(grenade->info.active) {
        if (grenade->info.direction == 1) {
            grenade->info.x++;
            if (grenade->info.x > GAME_WIDTH) {
                break;
            }
        }
        else if (grenade->info.direction == -1) {
            grenade->info.x--;
            if (grenade->info.x < -1) {
                break;
            }
        }

    pthread_mutex_unlock(&granades_mutex);
    writeMain(grenade->info); // Send updated position
    usleep(grenade->info.speed * 10000);  // Adjust delay for spee
}
    free(grenade); // Free grenade memory after thread completes    
    pthread_exit(0);
}

void terminateGrenades(Game *game) {

    pthread_mutex_lock(&granades_mutex); // Lock protecting shared resources

    for (int i = 0; i < MAX_GRENADES; i++) {
        if (game->grenades[i].info.active) {
            int cancel_result = pthread_cancel(game->grenades[i].thread);
            if (cancel_result != 0) {
                fprintf(stderr, "Error cancelling thread %d: %s\n", i, strerror(cancel_result));
            } else {
                int join_result = pthread_join(game->grenades[i].thread, NULL);
                if (join_result != 0) {
                    fprintf(stderr, "Error joining thread %d: %s\n", i, strerror(join_result));
                } else {
                    game->grenades[i].info.active = 0;
                    game->grenades[i].info.ID = -1;
                }
            }
        }
    }

    pthread_mutex_unlock(&granades_mutex); // Unlock
}

int findFreeGrenadeSlot(Game *game) {

    for (int i = 0; i < MAX_GRENADES; i++) {
        if (game->grenades[i].info.ID == -1 ) { // Check for inactive slots using ID
            return i , game->grenades[i].info.ID ;
        }
    }
    return -1; // No free slots
}

int checkCollisionProjectile(Informations obj1, Projectile obj2) {
//Frog and projectile collision check
  if (obj1.x < obj2.info.x + 1 &&
        obj1.x + 9 > obj2.info.x &&
        obj1.y < obj2.info.y + 1 &&
        obj1.y + 3 > obj2.info.y) {
        return 1; // Collision detected
    }
    return 0; // No collision
}

int isFrogOnCroc(Game *game) {

    //pthread_mutex_lock(&buffer_mutex);
    // for (int i = 0; i < N_CROC; i++) {
    //     // Check for overlap on the y-axis (same row)
    //     if (game->frog.info.y == game->crocodile[i].info.y) {
    //         // Check for overlap on the x-axis
    //         if (game->frog.info.x + FROG_WIDTH > game->crocodile[i].info.x &&
    //             game->frog.info.x < game->crocodile[i].info.x + CROC_LENGHT) {
    //             // Frog is on a crocodile
    //             game->frog.isOnCroc = 1;
    //             game->frog.onCrocIdx = i;
    //             game->frog.onCrocOffset = game->frog.info.x - game->crocodile[i].info.x; // Store offset
    //             //pthread_mutex_unlock(&buffer_mutex);
    //             return i + 1; // Return crocodile index +1 (ID)
    //         }
    //     }
    // }

    // game->frog.isOnCroc = 0; // Not on any crocodile
    // game->frog.onCrocIdx = -1; // Reset index
    // game->frog.onCrocOffset = 0; // Reset offset
    // //pthread_mutex_unlock(&buffer_mutex);
    // return 0; // Not on a crocodile

    for (int i = 0; i < N_CROC; i++) {
        if(checkCollision(game->frog.info, game->crocodile[i].info)) {
            game->frog.onCrocOffset = game->frog.info.x - game->crocodile[i].info.x;
            game->frog.isOnCroc = 1; 
            return i + 1; 
        }
    }
    game->frog.isOnCroc = 0; 
    return 0; 
}

int isFrogOnRiver(Game *game) {
    // The frog is in the water if it is on the lines where the crocodiles move
    if(game->frog.info.y < 9 && isFrogOnDen(game) == 0){
        return 1;
    }else{
        return 0;
    }
}
int isFrogOnTopBank(Game *game) {
	if (game->frog.info.y == 5)
		return 1;
	else
		return 0;
}

int isFrogOnDen(Game *game) {
    //pthread_mutex_lock(&buffer_mutex);
    Frog *frog = &game->frog;
    for (int i = 0; i < N_DENS; i++) {
        Den *den = &game->dens[i];
        if (den->isOpen &&
            frog->info.x >= den->x &&
            frog->info.x <= den->x + den->width - 2 && //Frog occupies 9 columns
            frog->info.y == den->y) {
            den->isOpen = 0;
           // pthread_mutex_unlock(&buffer_mutex);
            return 1;
        }
    }
    //pthread_mutex_unlock(&buffer_mutex);
    return 0;
}

int checkCollision(Informations frogInfo, Informations crocInfo) {
    if (frogInfo.y == crocInfo.y && 
        frogInfo.x >= crocInfo.x && 
        frogInfo.x <= (crocInfo.x + CROC_LENGHT)) {
            return 1; 
        }
    return 0; 
}
