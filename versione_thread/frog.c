#include "frog.h"


// Funzione per creare la rana
void createFrog(Game *game) {

    int result;

    game->frog.info.x = ((GAME_WIDTH - 1) / 2) - 4;
    game->frog.info.y = GAME_HEIGHT - 5;
    game->frog.info.ID = 0;
    game->frog.info.grenadesRemaining = 20;
    game->frog.lives = 3;
    game->frog.score = 0;
    game->frog.isOnCroc = 0;    
}
    
// Funzione per creare le granate
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

// Funzione per muovere le granate
void moveGrenade(void * params) {
    Grenade *grenade = (Grenade *)params;

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

    // Scrive le informazioni della granata nel buffer principale
    writeMain(grenade->info); 
    usleep(grenade->info.speed * 10000); 
    }

    grenade->info.active = 0; 
    grenade->info.ID = -1;
    pthread_exit(0);
}

// Funzione per terminare le granate attive
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

// Funzione per trovare uno slot libero per le granate
int findFreeGrenadeSlot(Game *game) {

    for (int i = 0; i < MAX_GRENADES; i++) {
        if (game->grenades[i].info.ID == -1 ) { 
            return i , game->grenades[i].info.ID ;
        }
    }
    return -1; // Nessuno slot libero trovato
}

// Funzione per verificare la collisione tra la rana e un proiettile
int checkCollisionProjectile(Informations obj1, Projectile obj2) {

    // Controlla se la rana (obj1) collide con il proiettile (obj2)
    if (obj1.x < obj2.info.x + 1 &&
        obj1.x + 9 > obj2.info.x &&
        obj1.y < obj2.info.y + 1 &&
        obj1.y + 3 > obj2.info.y) {
        return 1; 
    }
    return 0; 
}

// Funzione per verificare se la rana è su un coccodrillo
int isFrogOnCroc(Game *game) {
    bool wasOnCroc = game->frog.isOnCroc;

    for (int i = 0; i < N_CROC; i++) {
        if (checkCollision(game->frog.info, game->crocodile[i].info)) {
            
            // La rana è su un coccodrillo
            game->frog.isOnCroc = true;

            if (!wasOnCroc) {
                game->frog.onCrocOffset = game->frog.info.x - game->crocodile[i].info.x;
            }

            // Aggiorna la posizione della rana in base alla posizione del coccodrillo
            return i + 1; 
        }
    }

    // Se il ciclo finisce, la rana non è su nessun coccodrillo
    game->frog.isOnCroc = false;
    return 0;

}

// Funzione per verificare se la rana è nell'acqua, sulla riva superiore o sulla riva inferiore
int isFrogOnRiver(Game *game) {
    // La rana è "nell'acqua" solo tra la riga 14 (inizio del fiume) e la riga 51 (fine del fiume)
    if (game->frog.info.y > 14 && game->frog.info.y < 65) {
        return 1;  // La rana è nell'acqua
    }
    return 0;  // La rana non è nell'acqua
}

// Funzione per verificare se la rana è sulla riva superiore del fiume
int isFrogOnTopBank(Game *game) {
    if (game->frog.info.y >= 9 && game->frog.info.y <= 12) {
        return 1; 
    }
    return 0;
}

// Funzione per verificare se la rana è sulla riva inferiore del fiume
int isFrogOnTopRiver(Game *game) {
    if (game->frog.info.y < 9 && isFrogOnDen(game) == 0) {
        return 1; 
    }
    return 0; 
}

// Funzione per verificare se la rana è su una tana
int isFrogOnDen(Game *game) {
    Frog *frog = &game->frog;
    for (int i = 0; i < N_DENS; i++) {
        Den *den = &game->dens[i];
        // Controlla se la rana è sulla tana e se la tana è aperta
        if (den->isOpen &&
            frog->info.x >= den->x &&
            frog->info.x <= den->x + den->width - 2 &&  
            frog->info.y == den->y) {
            den->isOpen = 0;
            return 1;
        }
    }
    return 0;
}

// Funzione per verificare la collisione tra la rana e un coccodrillo
int checkCollision(Informations frogInfo, Informations crocInfo) {
    if (frogInfo.y == crocInfo.y && 
        frogInfo.x >= crocInfo.x && 
        frogInfo.x <= (crocInfo.x + CROC_LENGHT)) {
            return 1; 
        }
    return 0; 
}
