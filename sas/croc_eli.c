#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define FROG 1
#define CROC 2
#define MAX_V 5
#define MIN_V 3
#define ROW_FROG 3
#define COL_FROG 5
#define NUM_COCCODRILLI 20

typedef struct Player {
    unsigned int xP;
    unsigned int yP;
    char rana[ROW_FROG][COL_FROG];
    int id;
} Player;

typedef struct Coccodrillo {
    int x;
    int prevX;
    int y;
    pid_t pid;
    int id;
} Coccodrillo;

void fun_coccodrillo(int pipe_scrittura, int croc_id);
void move_player(int pipe_scrittura, Player *frog);
void print_frog(Player frog);
void init_frog(Player *frog);
void controllo(int pipeMain[2]);

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    int pipeMain[2];
    if (pipe(pipeMain) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Imposta le pipe come non bloccanti
    fcntl(pipeMain[0], F_SETFL, O_NONBLOCK);
    fcntl(pipeMain[1], F_SETFL, O_NONBLOCK);

    Player frog;
    init_frog(&frog);

    int pidF = fork();
    if (pidF < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pidF == 0) {
        close(pipeMain[0]);
        move_player(pipeMain[1], &frog);
        exit(0);
    }

    pid_t pidC[NUM_COCCODRILLI];
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        if ((pidC[i] = fork()) == 0) {
            close(pipeMain[0]);
            fun_coccodrillo(pipeMain[1], i);
            exit(0);
        }
    }

    close(pipeMain[1]);
    controllo(pipeMain);

    // Termina tutti i processi figli
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        kill(pidC[i], SIGTERM);
    }

    kill(pidF, SIGTERM);

    // Aspetta che tutti i processi figli terminino
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        waitpid(pidC[i], NULL, 0);
    }
    waitpid(pidF, NULL, 0);

    endwin();
    return 0;
}

void fun_coccodrillo(int pipe_scrittura, int croc_id) {
    Coccodrillo croc;
    croc.pid = getpid();
    srand(time(NULL) ^ croc.pid);
    croc.y = croc_id * (LINES / NUM_COCCODRILLI) + rand() % (LINES / NUM_COCCODRILLI);
    croc.x = (croc_id % 2 == 0) ? 0 : COLS - 1;
    croc.prevX = croc.x;
    croc.id = CROC;

    while (1) {
        croc.prevX = croc.x;
        if (croc_id % 2 == 0) {
            croc.x++;
            if (croc.x >= COLS - 1) {
                croc.x = 0;
            }
        } else {
            croc.x--;
            if (croc.x <= 0) {
                croc.x = COLS - 1;
            }
        }
        write(pipe_scrittura, &croc, sizeof(Coccodrillo));
        usleep(50000);
    }
}

void move_player(int pipe_scrittura, Player *frog) {
    int action;

    frog->id = FROG;
    while (1) {
        action = getch();
        switch (action) {
            case KEY_UP:
                frog->yP = (frog->yP > 0) ? frog->yP - 1 : frog->yP;
                break;
            case KEY_DOWN:
                frog->yP = (frog->yP < LINES - ROW_FROG) ? frog->yP + 1 : frog->yP;
                break;
            case KEY_LEFT:
                frog->xP = (frog->xP > 0) ? frog->xP - 1 : frog->xP;
                break;
            case KEY_RIGHT:
                frog->xP = (frog->xP < COLS - COL_FROG) ? frog->xP + 1 : frog->xP;
                break;
        }
        write(pipe_scrittura, frog, sizeof(Player));
        usleep(20000);
    }
}

void print_frog(Player frog) {
    for (int i = 0; i < ROW_FROG; i++) {
        for (int j = 0; j < COL_FROG; j++) {
            mvaddch(frog.yP + i, frog.xP + j, frog.rana[i][j]);
        }
    }
}

void init_frog(Player *frog) {
    char temp[ROW_FROG][COL_FROG] = {
        {'\\','@','^','@','/'},
        {'(',' ',' ',' ',')'},
        {'u','-','v','-','u'}
    };

    for (int i = 0; i < ROW_FROG; i++) {
        for (int j = 0; j < COL_FROG; j++) {
            frog->rana[i][j] = temp[i][j];
        }
    }

    frog->yP = LINES - ROW_FROG;
    frog->xP = COLS / 2 - (COL_FROG / 2);
}

void controllo(int pipeMain[2]) {
    Player frog;
    Coccodrillo croc;

    while (1) {
        int bytesRead = read(pipeMain[0], &frog, sizeof(Player));
        if (bytesRead > 0 && frog.id == FROG) {
            clear();
            print_frog(frog);
        }

        bytesRead = read(pipeMain[0], &croc, sizeof(Coccodrillo));
        if (bytesRead > 0 && croc.id == CROC) {
            mvprintw(croc.y, croc.prevX, "    ");
            mvprintw(croc.y, croc.x, "****");
        }

        refresh();
        usleep(20000);
    }
}