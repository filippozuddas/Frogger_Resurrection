#include "game.h"

void initGame() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    bkgd(COLOR_PAIR(1));

    int pipeFd[2];
    if (pipe(pipeFd) == -1) {
        perror("errore creazione pipe");
        exit(-1);
    }
}