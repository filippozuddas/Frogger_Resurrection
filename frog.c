#include "frog.h"

void createFrog(Frog *frog, int *pipeFd, int *mainToEntPipe) {
    /* inizializzazione frog */
    /* le coordinate x e y si riferiscono all'angolo in alto a sinistra della sprite */
    frog->info.x = (COLS - 1)/2 - 4; 
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
        close(pipeFd[0]); 
        inputHandler(frog, pipeFd, mainToEntPipe); 
        exit(0); 
    }
}

void inputHandler(Frog *frog, int *pipeFd, int *mainToEntPipe) { 
    Informations crocInfo;

    while(1) {

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
            default:
                continue;

            /* da implementare '' per le granate */
        }
        
        if(read(mainToEntPipe[0], &crocInfo, sizeof(Informations)) > 0){
            frog->info.x = crocInfo.x ;//+ frog->onCrocOffset; 
            //write(pipeFd[1], &frog->info, sizeof(Informations));
            //usleep(16000);
            //continue; // Salta il resto del ciclo
            
        }

        write(pipeFd[1], &frog->info, sizeof(Informations)); 

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

int isFrogOnCroc(Frog *frog, Crocodile *croc) {
    for (int i = 0; i < N_CROC; i++) {
        if(checkCollision(frog->info, croc[i].info)) {
            frog->onCrocOffset = frog->info.x - croc[i].info.x;
            frog->isOnCroc = 1; 
            return i + 1; 
        }
    }
    frog->isOnCroc = 0; 
    return 0; 
}
