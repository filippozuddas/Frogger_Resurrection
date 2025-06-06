#include "frog.h"

void createFrog(Game *game) {
    /* inizializzazione frog */
    /* le coordinate x e y si riferiscono all'angolo in alto a sinistra della sprite */
    
    Frog *frog = &game->frog; 

    frog->info.x = ((GAME_WIDTH - 1) / 2) - 4; 
    frog->info.y = GAME_HEIGHT - 5; 
    frog->info.ID = 0;
    frog->info.grenadesRemaining = 5; 
    frog->lives = 3; 
    frog->score = 0; 
    frog->isOnCroc = 0; 

    // pid_t pidFrog = fork(); 

    // if (pidFrog < 0) {
    //     perror("Fork failed"); 
    //     exit(1); 
    // }
    // else if(pidFrog == 0) {
    //     runInputClient();
    //     exit(0); 
    // }
    // else {
    //     frog->info.pid = pidFrog;
    //     //fprintf(stderr, "[PADRE Rana] Creata rana, PID figlio: %d\n", pidFrog); // DEBUG
    // }
}

// void inputHandler(Game *game) { 

//       while(1) {

//         readData(game->mainToFrogPipe[0], &game->frog.info, sizeof(Informations));

//         int input = getch(); 
        
//         switch(input) {
//             case 'w':
//             case 'W':
//             case KEY_UP:
//                 game->frog.info.y = (game->frog.info.y > 0) ? game->frog.info.y - FROG_HEIGHT : game->frog.info.y;
//                 break;
//             case 's':
//             case 'S':
//             case KEY_DOWN:
//                 game->frog.info.y = (game->frog.info.y < GAME_HEIGHT - FROG_HEIGHT - 1) ? game->frog.info.y + FROG_HEIGHT : game->frog.info.y;
//                 break;
//             case 'd':
//             case 'D':
//             case KEY_RIGHT:
//                 game->frog.info.x = (game->frog.info.x < GAME_WIDTH - FROG_WIDTH) ? game->frog.info.x + 1 : game->frog.info.x;
//                 break;
//             case 'a':
//             case 'A':
//             case KEY_LEFT:
//                 game->frog.info.x = (game->frog.info.x > 0) ? game->frog.info.x - 1 : game->frog.info.x;
//                 break;
//             case ' ': 
//                 if (game->frog.info.grenadesRemaining > 0) {
//                     Informations grenadeSignal;
//                     grenadeSignal.ID = -1; // -1 per granata a destra
//                     writeData(game->pipeFd[1], &grenadeSignal, sizeof(Informations));

//                     grenadeSignal.ID = -2; // -2 per granata a sinistra
//                     writeData(game->pipeFd[1], &grenadeSignal, sizeof(Informations));

//                     game->frog.info.grenadesRemaining--;
//                 }

//                 break;
//             default:
//                 continue;
//         }
        
//         writeData(game->pipeFd[1], &game->frog.info, sizeof(Informations)); 
//     }
// }

// void killFrog(Game *game) {
//     kill(game->frog.info.pid, SIGKILL);
//     waitpid(game->frog.info.pid, NULL, 0);
// }


int checkCollision(Informations frogInfo, Informations crocInfo) {
    if (frogInfo.y == crocInfo.y && 
        frogInfo.x >= crocInfo.x && 
        frogInfo.x <= (crocInfo.x + CROC_LENGHT)) {
            return 1; 
        }
    return 0; 
}

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

int isFrogOnRiver(Game *game) {
    // La rana è "nell'acqua" solo tra la riga 14 (inizio del fiume) e la riga 51 (fine del fiume)
    if (game->frog.info.y > 14 && game->frog.info.y < 65) {
        return 1;  // La rana è nell'acqua
    }
    return 0;  // La rana non è nell'acqua
}


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

void terminateGrenades(Game *game) {
    for (int i = 0; i < MAX_GRENADES; i++) {
        if (game->grenades[i].info.ID != -1) {
            kill(game->grenades[i].info.pid, SIGKILL);
            waitpid(game->grenades[i].info.pid, NULL, 0); //  terminazione
            game->grenades[i].info.ID = -1; // Libera lo slot
        }
    }
}

int checkCollisionProjectile(Informations entity, Projectile projectile) {
    if ((entity.y == projectile.info.y || entity.y + 2 == projectile.info.y) &&
         entity.x + FROG_WIDTH >= projectile.info.x && 
         entity.x <= projectile.info.x +1) {
            return 1; 
         }
    return 0; 
}