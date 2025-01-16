#include "sprite.h"

void printCroc(int x, int y, int direction) {
    wchar_t crocSprite[CROC_HEIGHT][CROC_LENGHT] = {
        {L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'▄', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'▄', L'▄', L'▄', L' ', L'▄'}, 
        {L'n', L'▄', L'▄', L'▄', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'▀'}, 
        {L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'n', L'n'}
    };

    short colors[CROC_HEIGHT][CROC_LENGHT] = {
        {BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BLACK_BG,    BLACK_BG,    BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG},
        {BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BG_BG,       BLACK_BG,    BLACK_BG,    BLACK_BG,    GREEN_BLACK, GREEN_BLACK, GREEN_BLACK, GREEN_BLACK, BLACK_WHITE, WHITE_WHITE, BLACK_BLACK, BLACK_BG,    BLACK_BG,    BLACK_BG,    GREEN_BLACK, BLACK_BG},
        {BG_BG,       BLACK_BG,    BLACK_BG,    BLACK_BG,    GREEN_BLACK, GREEN_BLACK, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, GREEN_GREEN, BLACK_GREEN, WHITE_GREEN, BLACK_GREEN, WHITE_GREEN, BLACK_GREEN, WHITE_GREEN, BLACK_GREEN, BG_BLACK},
        {BLACK_BLACK, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_GREEN, BLACK_BLACK, BG_BG,       BG_BG}
    }; 

    if (direction == 1) {
        for (int i = 0; i < CROC_HEIGHT; i++) {
            for (int j = (CROC_LENGHT-1); j >= 0; j--) {
                short xx = x + (CROC_LENGHT - j); 
                if(crocSprite[i][j] != L'n'){
                    CHANGE_COLOR(colors[i][j]); 
                    mvaddch(y+i, xx, crocSprite[i][j]); 
                }
            }
        }
    }
    else {
        for (int i = 0; i < CROC_HEIGHT; i++) {
            for (int j = 0; j < CROC_LENGHT; j++) {
                short xx = x + j + 1; 
                if(crocSprite[i][j] != L'n'){
                    CHANGE_COLOR(colors[i][j]); 
                    mvaddch(y+i, xx, crocSprite[i][j]); 
                }
            }
        }
    }

}

void printFrog(int x, int y) {
    char frogSprite[FROG_HEIGHT][FROG_LENGTH] = {
        {' ', '/', 'o', ' ', 'o', '\\', ' '}, 
        {'(', ' ', ' ', '^', ' ', ' ', ')'},
        {'(', ' ', ' ', ' ', ' ', ' ', ')'},
        {' ', '/', ' ', ' ', ' ', '\\', ' '}
    };

    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    int maxY, maxX; 
    getmaxyx(stdscr, maxY, maxX); 

    attron(1); 
    for (int i = 0; i < FROG_HEIGHT; i++) {
        for (int j = 0; j < FROG_LENGTH; j++) {
            if(y+i < maxY && x+j < maxX)
                mvaddch(y + i, x + j, frogSprite[i][j]);
        }
    }
    attroff(1); 
}


//  ▀ █ ▄