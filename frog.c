#include "frog.h"

void createFrog(Frog *frog, int pipeFd[]) {
    //inizializzazione frog
    frog->coords.x = (COLS - 1)/2; 
    frog->coords.y = LINES - 5; 
    frog->coords.ID = 0;
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
    while(1) {
        int input = getch(); 
        if(input != ERR) {
            switch(input) {
                case 'w':
                case 'W':
                case KEY_UP:
                    frog->coords.y = (frog->coords.y > 0) ? frog->coords.y - 1 : frog->coords.y;
                    break;
                case 's':
                case 'S':
                case KEY_DOWN:
                    frog->coords.y = (frog->coords.y < LINES - FROG_HEIGHT) ? frog->coords.y + 1 : frog->coords.y;
                    break;
                case 'd':
                case 'D':
                case KEY_RIGHT:
                    frog->coords.x = (frog->coords.x < COLS - 1) ? frog->coords.x + 1 : frog->coords.x;
                    break;
                case 'a':
                case 'A':
                case KEY_LEFT:
                    frog->coords.x = (frog->coords.x > 0) ? frog->coords.x - 1 : frog->coords.x;
                    break;
            }
        }

        // MessageType type = MSG_FROG;
        // write(pipeFd[1], &type, sizeof(type)); 
        write(pipeFd[1], &frog->coords, sizeof(Coordinates)); 

        //usleep(100000);
    }
}