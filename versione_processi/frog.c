#include "frog.h"

// Inizializza i valori della rana (player)
void createFrog(Game *game) {
    /* inizializzazione frog */
    /* le coordinate x e y si riferiscono all'angolo in alto a sinistra della sprite */
    
    Frog *frog = &game->frog; 

    frog->info.x = ((GAME_WIDTH - 1) / 2) - 4; 
    frog->info.y = GAME_HEIGHT - 5; 
    frog->info.ID = 0;
    frog->info.grenadesRemaining = 10; 
    frog->lives = 3; 
    frog->score = 0; 
    frog->isOnCroc = 0; 
}

// Controlla se la posizione della rana coincide con quella dell'hitbox di un coccodrillo 
int checkCollision(Informations frogInfo, Informations crocInfo) {
    if (frogInfo.y == crocInfo.y && 
        frogInfo.x >= crocInfo.x && 
        frogInfo.x <= (crocInfo.x + CROC_LENGHT)) {
            return 1; 
        }
    return 0; 
}

// Controlla se la rana è su un coccodrillo
int isFrogOnCroc(Game *game) {
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

// Controlla se la rana è nell'acqua (fiume principale)
int isFrogOnRiver(Game *game) {
    if (game->frog.info.y > 14 && game->frog.info.y < 65) {
        return 1;  
    }
    return 0;  
}

// Controlla se la rana è su una delle tane
int isFrogOnDen(Game *game) {
    Frog *frog = &game->frog;

    for (int i = 0; i < N_DENS; i++) {
        Den *den = &game->dens[i];

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


// Controlla se la rana è sulla sponda superiore
int isFrogOnTopBank(Game *game) {
    if (game->frog.info.y >= 9 && game->frog.info.y <= 12) {
        return 1; 
    }
    return 0;
}

// Controlla se la rana è caduta nell'acqua sopra la sponda superiore
int isFrogOnTopRiver(Game *game) {
    if (game->frog.info.y < 9 && isFrogOnDen(game) == 0) {
        return 1; 
    }
    return 0; 
}

// Inizializza una granata e crea il relativo processo 
void createGrenade(Game *game, int direction, int grenadeId, int grenadeIndex) {
    Grenade grenade; 

    grenade.info.x = (direction == 1) ? game->frog.info.x + FROG_WIDTH - 1 : game->frog.info.x - 1; 
    grenade.info.y = game->frog.info.y + 2; 
    grenade.info.direction = direction; 
    grenade.info.speed = 3; 
    grenade.info.ID = grenadeId; 

    pid_t grenadePid = fork();
    if (grenadePid < 0) {
        perror("Fork failed"); 
        exit(1);
    }
    else if (grenadePid == 0) {
        moveGrenade(&grenade, game, grenadeId); 
        exit(0);
    }
    else {
        grenade.info.pid = grenadePid; 
        game->grenades[grenadeIndex] = grenade;
    }

}

// Funzione che gestisce il movimento della granata
void moveGrenade(Grenade *grenade, Game *game, int grenadeIndex) {
    while(1) {
        if (grenade->info.direction == 1) {
            grenade->info.x++;
            if (grenade->info.x > GAME_WIDTH) {
                break;
            }
        }
        else if (grenade->info.direction == -1) {
            grenade->info.x--;
            if (grenade->info.x < -2) {
                break;
            }
        }

        writeData(game->pipeFd[1], &grenade->info, sizeof(Informations));
        usleep(grenade->info.speed * 10000);
    }
    _exit(0);
}

// Gestisce la terminazione delle granate, terminando i processi e liberando gli slot nell'array
void terminateGrenades(Game *game) {
    for (int i = 0; i < MAX_GRENADES; i++) {
        if (game->grenades[i].info.ID != -1) {
            kill(game->grenades[i].info.pid, SIGKILL);
            waitpid(game->grenades[i].info.pid, NULL, 0); 
            game->grenades[i].info.ID = -1; 
        }
    }
}

// Controlla se la rana o una granata è stata colpita da un proiettile
int checkCollisionProjectile(Informations entity, Projectile projectile) {
    if ((entity.y == projectile.info.y || entity.y + 2 == projectile.info.y) &&
         entity.x + FROG_WIDTH >= projectile.info.x && 
         entity.x <= projectile.info.x +1) {
            return 1; 
         }
    return 0; 
}