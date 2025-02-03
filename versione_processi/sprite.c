#include "sprite.h"

void printCroc(WINDOW *win, int x, int y, int direction) {
    wchar_t crocSprite[CROC_HEIGHT][CROC_LENGHT] = {
        {L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L' ', L'▄', L'▄', L'▄', L'▄', L'▄'}, 
        {L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}, 
        {L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'n', L'n'}
    };

    short colors[CROC_HEIGHT][CROC_LENGHT] = {
        {0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       BLACK_BG,    BLACK_BG,    0,       0,       0,       0,       0,       0},
        {0,       0,       0,       0,       0,       0,       BLACK_BG,    BLACK_BG,    BLACK_BG,    GREEN_BLACK, GREEN_BLACK, GREEN_BLACK, GREEN_BLACK, BLACK_WHITE, WHITE_WHITE, BLACK_BLACK, BLACK_BG,    BLACK_BG,    BLACK_BG,    GREEN_BLACK, BLACK_BG},
        {0,       BLACK_BG,    BLACK_BG,    BLACK_BG,    GREEN_BLACK, GREEN_BLACK, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, BLACK_GREEN, WHITE_GREEN, BLACK_GREEN, WHITE_GREEN, BLACK_GREEN, WHITE_GREEN, BLACK_GREEN, BLACK_BG},
        {BLACK_BLACK, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_BLACK, 0,       0}
    }; 


    if (direction == 0) {   //vanno a destra 
        for (int i = 0; i < CROC_HEIGHT; i++) {
            for (int j = 0; j < CROC_LENGHT; j++) {
                short xx = x + j + 1; 
                if(crocSprite[i][j] != L'n'){
                    wattron(win, COLOR_PAIR(colors[i][j]));
                    mvwprintw(win, y+i, xx, "%lc", crocSprite[i][j]); 
                    wattroff(win, COLOR_PAIR(colors[i][j]));
                }
            }
        }
    }
    else {  //vanno a sinistra 
        for (int i = 0; i < CROC_HEIGHT; i++) {
            for (int j = (CROC_LENGHT-1); j >= 0; j--) {
                short xx = x + (CROC_LENGHT - j); 
                if(crocSprite[i][j] != L'n'){
                    wattron(win, COLOR_PAIR(colors[i][j]));
                    mvwprintw(win, y+i, xx, "%lc", crocSprite[i][j]); 
                    wattroff(win, COLOR_PAIR(colors[i][j]));
                }
            }
        }
    }

}

void printFrog(WINDOW *win, int x, int y) {
    wchar_t frogSprite[FROG_HEIGHT][FROG_WIDTH] = {
        {L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'n'},
        {L' ', L' ', L' ', L'▄', L' ', L' ', L' ', L'▄', L' ', L' '},
        {L' ', L' ', L' ', L' ', L'▄', L'▄', L' ', L' ', L' ', L' '},
        {L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'n'}
    };

    short colors[FROG_HEIGHT][FROG_WIDTH] = {
        {0, BLACK_BG, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_BG, BLACK_BG, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_BG, 0},
        {BLACK_BLACK, GREEN_GREEN_FROG, BLACK_BLACK, BLACK_WHITE, GREEN_GREEN_FROG, GREEN_GREEN_FROG, BLACK_BLACK, BLACK_WHITE, GREEN_GREEN_FROG, BLACK_BLACK},
        {BLACK_BLACK, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_PINK_FROG, GREEN_PINK_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, BLACK_BLACK},
        {0, BLACK_BG, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_BG, BLACK_BG, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_BG, 0}
    };

    /*short colors[FROG_HEIGHT][FROG_WIDTH] = {
        {0, YELLOW_GREEN_FROG, GREEN_BLACK_FROG, GREEN_BLACK_FROG, YELLOW_GREEN_FROG, YELLOW_GREEN_FROG, GREEN_BLACK_FROG, GREEN_BLACK_FROG, YELLOW_GREEN_FROG, 0},
        {BLACK_BLACK, GREEN_GREEN_FROG, BLACK_BLACK, BLACK_WHITE, GREEN_GREEN_FROG, GREEN_GREEN_FROG, BLACK_BLACK, BLACK_WHITE, GREEN_GREEN_FROG, BLACK_BLACK},
        {BLACK_BLACK, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_PINK_FROG, GREEN_PINK_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, BLACK_BLACK},
        {0, YELLOW_GREEN_FROG, GREEN_BLACK_FROG, GREEN_BLACK_FROG, YELLOW_GREEN_FROG, YELLOW_GREEN_FROG, GREEN_BLACK_FROG, GREEN_BLACK_FROG, YELLOW_GREEN_FROG, 0}
    };*/

    int maxY, maxX; 
    getmaxyx(win, maxY, maxX); 

    for (int i = 0; i < FROG_HEIGHT; i++) {
        for (int j = 0; j < FROG_WIDTH; j++) {
            if(frogSprite[i][j] != L'n'){
                wattron(win, COLOR_PAIR(colors[i][j]));
                mvwprintw(win, y + i, x + j, "%lc", frogSprite[i][j]);
            }
        }

    } 
}


//  ▀ █ ▄