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

void printFrog(Game *game, WINDOW *win, int x, int y) {
    wchar_t frogSprite[FROG_HEIGHT][FROG_WIDTH] = {
        {L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'n'},
        {L' ', L' ', L' ', L'▄', L' ', L' ', L' ', L'▄', L' ', L' '},
        {L' ', L' ', L' ', L' ', L'▄', L'▄', L' ', L' ', L' ', L' '},
        {L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'n'}
    };

    short colors[FROG_HEIGHT][FROG_WIDTH] = {
        {0, BLACK_YELLOW_GRASS, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_YELLOW_GRASS, BLACK_YELLOW_GRASS, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_YELLOW_GRASS, 0},
        {BLACK_BLACK, GREEN_GREEN_FROG, BLACK_BLACK, BLACK_WHITE, GREEN_GREEN_FROG, GREEN_GREEN_FROG, BLACK_BLACK, BLACK_WHITE, GREEN_GREEN_FROG, BLACK_BLACK},
        {BLACK_BLACK, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_PINK_FROG, GREEN_PINK_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, BLACK_BLACK},
        {0, BLACK_YELLOW_GRASS, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_YELLOW_GRASS, BLACK_YELLOW_GRASS, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_YELLOW_GRASS, 0}
    };

    if (isFrogOnCroc(game)) {
        colors[0][1] = BLACK_BLUE;
        colors[0][4] = BLACK_BLUE; 
        colors[0][5] = BLACK_BLUE; 
        colors[0][8] = BLACK_BLUE;

        colors[3][1] = BLACK_BLACK_CROC;
        colors[3][4] = BLACK_BLACK_CROC; 
        colors[3][5] = BLACK_BLACK_CROC; 
        colors[3][8] = BLACK_BLACK_CROC;
    }

    if (isFrogOnTopBank(game)) {
        colors[0][1] = BLACK_GREEN_GRASS;
        colors[0][4] = BLACK_GREEN_GRASS; 
        colors[0][5] = BLACK_GREEN_GRASS; 
        colors[0][8] = BLACK_GREEN_GRASS;

        colors[3][1] = BLACK_GREEN_GRASS;
        colors[3][4] = BLACK_GREEN_GRASS; 
        colors[3][5] = BLACK_GREEN_GRASS; 
        colors[3][8] = BLACK_GREEN_GRASS;
    }

    for (int i = 0; i < FROG_HEIGHT; i++) {
        for (int j = 0; j < FROG_WIDTH; j++) {
            if(frogSprite[i][j] != L'n'){
                wattron(win, COLOR_PAIR(colors[i][j]));
                mvwprintw(win, y + i, x + j, "%lc", frogSprite[i][j]);
                wattroff(win, COLOR_PAIR(colors[i][j]));
            }
        }
    } 
}

void printFrogOnDen(WINDOW *win, int x, int y) {
    wchar_t frogSprite[FROG_HEIGHT][FROG_WIDTH] = {
        {L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'n'},
        {L' ', L' ', L' ', L'▄', L' ', L' ', L' ', L'▄', L' ', L' '},
        {L' ', L' ', L' ', L' ', L'▄', L'▄', L' ', L' ', L' ', L' '},
        {L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'n'}
    };

    short colors[FROG_HEIGHT][FROG_WIDTH] = {
        {0, BLACK_BLUE, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_BLUE, BLACK_BLUE, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_BLUE, 0},
        {BLACK_BLACK, GREEN_GREEN_FROG, BLACK_BLACK, BLACK_WHITE, GREEN_GREEN_FROG, GREEN_GREEN_FROG, BLACK_BLACK, BLACK_WHITE, GREEN_GREEN_FROG, BLACK_BLACK},
        {BLACK_BLACK, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_PINK_FROG, GREEN_PINK_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, GREEN_GREEN_FROG, BLACK_BLACK},
        {0, BLACK_BLUE, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_BLUE, BLACK_BLUE, GREEN_BLACK_FROG, GREEN_BLACK_FROG, BLACK_BLUE, 0}
    };

    for (int i = 0; i < FROG_HEIGHT; i++) {
        for (int j = 0; j < FROG_WIDTH; j++) {
            if(frogSprite[i][j] != L'n'){
                wattron(win, COLOR_PAIR(colors[i][j]));
                mvwprintw(win, y + i, x + j, "%lc", frogSprite[i][j]);
                wattroff(win, COLOR_PAIR(colors[i][j]));
            }
        }
    } 
}

void printGrenades(Game *game) {
    wchar_t grenadeSprite[1][2] = {
        {L'▄', L'▄'}
    };

    short colors[1][2] = {
        {LIGHT_DARK_GREY, MID_GREY_BLACK}
    };

    for (int k = 0; k < MAX_GRENADES; k++) {
        if (game->grenades[k].info.active == 1) {
            for (int i = 0; i < 1; i++) {
                for (int j = 0; j < 2; j++) {
                    if(grenadeSprite[i][j] != L'n'){
                        wattron(game->gameWin, COLOR_PAIR(colors[i][j]));
                        mvwprintw(game->gameWin, game->grenades[k].info.y + i, game->grenades[k].info.x + j, "%lc", grenadeSprite[i][j]);
                        wattroff(game->gameWin, COLOR_PAIR(colors[i][j]));
                    }
                }
            } 
        }
    }
}

void printProjectiles(Game *game) {
    wchar_t projectileSprite[1][3] = {
        {L' ', L' ', L' '}
    };

    short colors[1][3] = {
        {YELLOW_YELLOW_PROJ, LIGHT_LIGHT_PROJ, ORANGE_ORANGE_PROJ}
    };

    for (int k = 0; k < MAX_PROJECTILES; k++) {
        if (game->projectiles[k].info.active == 1) {
            for (int i = 0; i < 1; i++) {
                for (int j = 0; j < 3; j++) {
                    if(projectileSprite[i][j] != L'n'){
                        wattron(game->gameWin, COLOR_PAIR(colors[i][j]));
                        mvwprintw(game->gameWin, game->projectiles[k].info.y + i, game->projectiles[k].info.x + j, "%lc", projectileSprite[i][j]);
                        wattroff(game->gameWin, COLOR_PAIR(colors[i][j]));
                    }
                }
            } 
        }
    }
}

void printDen(Game *game, int x, int y) {
    wchar_t denSprite[FROG_HEIGHT][FROG_WIDTH] = {
        {L'n', L'▄', L'▄', L'▄', L'n', L'n', L'▄', L'▄', L'▄', L'n'},
        {L' ', L' ', L' ', L' ', L'▄', L'▄', L' ', L' ', L' ', L' '},
        {L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' '},
        {L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'n'}
    };

    short colors[FROG_HEIGHT][FROG_WIDTH] = {
        {0, DARK_GREEN_BLUE, LIGHT_GREEN_DARK_GREEN, DARK_GREEN_BLUE, 0, 0, DARK_GREEN_BLUE, LIGHT_GREEN_DARK_GREEN, DARK_GREEN_BLUE, 0},
        {DARK_GREEN_DARK_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_DARK_GREEN, LIGHT_GREEN_DARK_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, DARK_GREEN_DARK_GREEN},
        {DARK_GREEN_DARK_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_LIGHT_GREEN, DARK_GREEN_DARK_GREEN},
        {0, DARK_GREEN_BLUE, LIGHT_GREEN_DARK_GREEN, LIGHT_GREEN_DARK_GREEN, LIGHT_GREEN_DARK_GREEN, LIGHT_GREEN_DARK_GREEN, LIGHT_GREEN_DARK_GREEN, LIGHT_GREEN_DARK_GREEN, DARK_GREEN_BLUE, 0}
    };

    for (int i = 0; i < FROG_HEIGHT; i++) {
        for (int j = 0; j < FROG_WIDTH; j++) {
            if(denSprite[i][j] != L'n'){
                wattron(game->gameWin, COLOR_PAIR(colors[i][j]));
                mvwprintw(game->gameWin, y + i, x + j, "%lc", denSprite[i][j]);
                wattroff(game->gameWin, COLOR_PAIR(colors[i][j]));
            }
        }
    } 
}

MenuOption menu[MENU_ITEMS] = {
    {1, {
        L"██████╗░██╗░░░░░░█████╗░██╗░░░██╗",
        L"██╔══██╗██║░░░░░██╔══██╗╚██╗░██╔╝",
        L"██████╔╝██║░░░░░███████║░╚████╔╝░",
        L"██╔═══╝░██║░░░░░██╔══██║░░╚██╔╝░░",
        L"██║░░░░░███████╗██║░░██║░░░██║░░░"
    }},
    {2, {
        L"░█████╗░██╗░░██╗░█████╗░░█████╗░░██████╗███████╗",
        L"██╔══██╗██║░░██║██╔══██╗██╔══██╗██╔════╝██╔════╝",
        L"██║░░╚═╝███████║██║░░██║██║░░██║╚█████╗░█████╗░░",
        L"██║░░██╗██╔══██║██║░░██║██║░░██║░╚═══██╗██╔══╝░░",
        L"╚█████╔╝██║░░██║╚█████╔╝╚█████╔╝██████╔╝███████╗"
    }},
    {3, {
        L"░██████╗░█████╗░░█████╗░██████╗░███████╗░██████╗",
        L"██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔════╝",
        L"╚█████╗░██║░░╚═╝██║░░██║██████╔╝█████╗░░╚█████╗░",
        L"░╚═══██╗██║░░██╗██║░░██║██╔══██╗██╔══╝░░░╚═══██╗",
        L"██████╔╝╚█████╔╝╚█████╔╝██║░░██║███████╗██████╔╝"
    }},
    {4, {
        L"███████╗██╗░░██╗██╗████████╗",
        L"██╔════╝╚██╗██╔╝██║╚══██╔══╝",
        L"█████╗░░░╚███╔╝░██║░░░██║░░░",
        L"██╔══╝░░░██╔██╗░██║░░░██║░░░",
        L"███████╗██╔╝╚██╗██║░░░██║░░░"
    }}
};

MenuOption level[LEVEL_ITEMS] = {
    {1, {
        L"███████╗░█████╗░░██████╗██╗░░░██╗",
        L"██╔════╝██╔══██╗██╔════╝╚██╗░██╔╝",
        L"█████╗░░███████║╚█████╗░░╚████╔╝░",
        L"██╔══╝░░██╔══██║░╚═══██╗░░╚██╔╝░░",
        L"███████╗██║░░██║██████╔╝░░░██║░░░"
    }},
    {2, {
        L"███╗░░░███╗███████╗██████╗░██╗██╗░░░██╗███╗░░░███╗",
        L"████╗░████║██╔════╝██╔══██╗██║██║░░░██║████╗░████║",
        L"██╔████╔██║█████╗░░██║░░██║██║██║░░░██║██╔████╔██║",
        L"██║╚██╔╝██║██╔══╝░░██║░░██║██║██║░░░██║██║╚██╔╝██║",
        L"██║░╚═╝░██║███████╗██████╔╝██║╚██████╔╝██║░╚═╝░██║"
    }},
    {3, {
        L"██╗░░██╗░█████╗░██████╗░██████╗░",
        L"██║░░██║██╔══██╗██╔══██╗██╔══██╗",
        L"███████║███████║██████╔╝██║░░██║",
        L"██╔══██║██╔══██║██╔══██╗██║░░██║",
        L"██║░░██║██║░░██║██║░░██║██████╔╝"
    }}
};