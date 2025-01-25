#include "frog.h"

void createFrog(Game *game) {
    /* inizializzazione frog */
    /* le coordinate x e y si riferiscono all'angolo in alto a sinistra della sprite */
    
    Frog *frog = &game->frog; 

    frog->info.x = ((COLS - 1) / 2) - 4; 
    frog->info.y = LINES - 4; 
    frog->info.ID = 0;
    frog->lives = 3; 
    frog->score = 0; 
    frog->isOnCroc = 0; 

    pid_t pidFrog = fork(); 

    if (pidFrog < 0) {
        perror("Fork failed"); 
        exit(1); 
    }
    else if(pidFrog == 0) {
        close(game->pipeFd[0]); 
        inputHandler(game, frog); 
        exit(0); 
    }
}

void inputHandler(Game *game, Frog *frog) { 
    int grenadeId = 17; 
    while(1) {

        read(game->mainToFrogPipe[0], &frog->info, sizeof(Informations));

        int input = getch(); 
        
        switch(input) {
            case 'w':
            case 'W':
            case KEY_UP:
                frog->info.y = (frog->info.y > 0) ? frog->info.y - FROG_HEIGHT : frog->info.y;
                break;
            case 's':
            case 'S':
            case KEY_DOWN:
                frog->info.y = (frog->info.y < LINES - FROG_HEIGHT) ? frog->info.y + FROG_HEIGHT : frog->info.y;
                break;
            case 'd':
            case 'D':
            case KEY_RIGHT:
                frog->info.x = (frog->info.x < COLS - 1) ? frog->info.x + 1 : frog->info.x;
                break;
            case 'a':
            case 'A':
            case KEY_LEFT:
                frog->info.x = (frog->info.x > 0) ? frog->info.x - 1 : frog->info.x;
                break;
            case ' ': 
                //se a schermo è presente una sola granate viene stampata in modo fluido (da fixare)
                createGrenade(game, frog, 1, grenadeId); 
                createGrenade(game, frog, -1, grenadeId + 1); 
                grenadeId += 2;

                break;
            default:
                continue;
        }
        
        write(game->pipeFd[1], &frog->info, sizeof(Informations)); 

        //usleep(16000);
    }
}


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
    if(game->frog.info.y < LINES - 5 && game->frog.info.y > 10) {
        return 1;
    }
    return 0; 
}

void createGrenade(Game *game, Frog *frog, int direction, int grenadeId) {
    Grenade grenade; 

    grenade.info.x = (direction == 1) ? frog->info.x + FROG_LENGTH : frog->info.x; 
    grenade.info.y = frog->info.y + 2;
    grenade.info.direction = direction; 
    grenade.info.speed = 30000;
    grenade.info.ID = grenadeId;

    pid_t grenadePid = fork(); 
    if (grenadePid < 0) {
        perror("Fork failed"); 
        exit(1); 
    }
    else if (grenadePid == 0) {
        grenade.info.pid = getpid();
        moveGrenade(&grenade, game->pipeFd); 
        exit(0); 
    }
}

void moveGrenade(Grenade *grenade, int *pipeFd) {
    while(1) {
        if(grenade->info.direction == 1) {
            grenade->info.x++; 
            if(grenade->info.x > COLS) {
                break;
                //exit(0);
                //kill(grenade->info.pid, SIGTERM); 
            }
        } 
        else if (grenade->info.direction == -1) {
            grenade->info.x--; 
            if(grenade->info.x < -1) {
                break;
                //exit(0);
                //kill(grenade->info.pid, SIGTERM); 
            }
        }

        //mvprintw(grenade->info.y, grenade->info.x, "*");

        write(pipeFd[1], &grenade->info, sizeof(Informations)); 
        usleep(grenade->info.speed); 
    }

    _exit(0);
}
