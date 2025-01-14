#include "frog.h"

void createFrog(Frog *frog, int pipeFd[]) {
    //inizializzazione frog
    frog->x = (COLS - 1)/2; 
    frog->y = LINES - 5; 
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
        moveFrog(frog, pipeFd); 
        exit(0); 
    }
}

void moveFrog(Frog *frog, int pipeFd[]) {
    usleep(1000);
    while(1) {
        int input = getch(); 

        switch(input) {
            case 'w':
            case 'W':
            case KEY_UP:
                frog->y = (frog->y > 0) ? frog->y - 1 : frog->y;
                break;
            case 's':
            case 'S':
            case KEY_DOWN:
                frog->y = (frog->y < LINES - FROG_HEIGHT) ? frog->y + 1 : frog->y;
                break;
            case 'd':
            case 'D':
            case KEY_RIGHT:
                frog->x = (frog->x < COLS - 1) ? frog->x + 1 : frog->x;
                break;
            case 'a':
            case 'A':
            case KEY_LEFT:
                frog->x = (frog->x > 0) ? frog->x - 1 : frog->x;
                break;
            default:
                continue;
        }

        MessageType type = MSG_FROG;
        write(pipeFd[1], &type, sizeof(type)); 
        write(pipeFd[1], frog, sizeof(Frog)); 
    }
}