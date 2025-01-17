#include "frog.h"

void createFrog(Frog frog, int *pipeFd) {
    //inizializzazione frog
    frog.info.x = (COLS - 1)/2; 
    frog.info.y = LINES - 4; 
    frog.info.ID = 0;
    frog.lives = 3; 
    frog.score = 0; 
    frog.isOnCroc = 0; 
    

    pid_t pidFrog = fork(); 

    if (pidFrog < 0) {
        perror("Fork failed"); 
        exit(1); 
    }
    else if(pidFrog == 0) {
        close(pipeFd[0]); 
        inputHandler(frog, pipeFd); 
        exit(0); 
    }
}

void inputHandler(Frog frog, int *pipeFd) {
    while(1) {
        int input = getch(); 
        
        switch(input) {
            case 'w':
            case 'W':
            case KEY_UP:
                frog.info.y = (frog.info.y > 0) ? frog.info.y - FROG_HEIGHT : frog.info.y;
                break;
            case 's':
            case 'S':
            case KEY_DOWN:
                frog.info.y = (frog.info.y < LINES - FROG_HEIGHT) ? frog.info.y + FROG_HEIGHT : frog.info.y;
                break;
            case 'd':
            case 'D':
            case KEY_RIGHT:
                frog.info.x = (frog.info.x < COLS - 1) ? frog.info.x + 1 : frog.info.x;
                break;
            case 'a':
            case 'A':
            case KEY_LEFT:
                frog.info.x = (frog.info.x > 0) ? frog.info.x - 1 : frog.info.x;
                break;

            /* da implementare '' per le granate */
        }
        
        write(pipeFd[1], &frog.info, sizeof(Informations)); 

        usleep(16000);
    }
}