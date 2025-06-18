#include "frog.h"


void createFrog(Game *game) {

    int result;

    //Frog *frog = &game->frog;
    game->frog.info.x = ((GAME_WIDTH - 1) / 2) - 4;
    game->frog.info.y = GAME_HEIGHT - 5;
    game->frog.info.ID = 0;
    game->frog.info.grenadesRemaining = 20;
    game->frog.lives = 3;
    game->frog.score = 0;
    game->frog.isOnCroc = 0;    
}
    
void createGrenade(Game *game, int direction, int grenadeId, int grenadeIndex){
    Grenade *grenade = &game->grenades[grenadeIndex];

    grenade->info.x = (direction == 1) ? game->frog.info.x + FROG_WIDTH - 1 : game->frog.info.x - 1; 
    grenade->info.y = game->frog.info.y + 2; 
    grenade->info.direction = direction; 
    grenade->info.speed = 3; 
    grenade->info.ID = grenadeId; 
    grenade->info.active = 1;

    pthread_create(&grenade->thread, NULL, moveGrenade, (void*)grenade);

}

void moveGrenade(void * params) {
    Grenade *grenade = (Grenade *)params;

    //pthread_mutex_lock(&granades_mutex);
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

    //pthread_mutex_unlock(&granades_mutex);
    writeMain(grenade->info); // Send updated position
    usleep(grenade->info.speed * 10000);  // Adjust delay for spee
    }

    //free(grenade); // Free grenade memory after thread completes    
    grenade->info.active = 0; 
    grenade->info.ID = -1;
    pthread_exit(0);
}

void terminateGrenades(Game *game) {
    for (int i = 0; i < MAX_GRENADES; i++) {
        if (game->grenades[i].info.active) {
            pthread_cancel(game->grenades[i].thread);
            pthread_join(game->grenades[i].thread, NULL); 
            game->grenades[i].info.active = 0; 
            game->grenades[i].info.ID = -1;
        }
    }
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
    bool wasOnCroc = game->frog.isOnCroc;

    for (int i = 0; i < N_CROC; i++) {
        if (checkCollision(game->frog.info, game->crocodile[i].info)) {
            
            // La rana è su un coccodrillo
            game->frog.isOnCroc = true;

            // Calcola l'offset SOLO se la rana non era già su un coccodrillo nel frame precedente.
            // Questo succede solo nel momento esatto in cui atterra.
            if (!wasOnCroc) {
                game->frog.onCrocOffset = game->frog.info.x - game->crocodile[i].info.x;
            }
            
            return i + 1; // Ritorna l'ID del coccodrillo su cui si trova
        }
    }

    // Se il ciclo finisce, la rana non è su nessun coccodrillo
    game->frog.isOnCroc = false;
    return 0;

}

int isFrogOnRiver(Game *game) {
    // La rana è "nell'acqua" solo tra la riga 14 (inizio del fiume) e la riga 51 (fine del fiume)
    if (game->frog.info.y > 14 && game->frog.info.y < 65) {
        return 1;  // La rana è nell'acqua
    }
    return 0;  // La rana non è nell'acqua
}

int isFrogOnTopBank(Game *game) {
    if (game->frog.info.y >= 9 && game->frog.info.y <= 12) {
        return 1; 
    }
    return 0;
}

int isFrogOnTopRiver(Game *game) {
    if (game->frog.info.y < 9 && isFrogOnDen(game) == 0) {
        return 1; 
    }
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
