#include "colors.h"

void initColors(short c, short r, short g, short b) {
    init_color(c, r * RGB_CONVERTER, g * RGB_CONVERTER, b * RGB_CONVERTER);  
}

void setColors() {


    /* Crocodile */
    initColors(CROC_GREEN, 173,255,47);
    initColors(CROC_WHITE, 255,250,250); 
    initColors(CROC_BLACK, 0, 0, 0); 

    init_pair(BLACK_BG, CROC_BLACK, COLOR_BLUE); 
    init_pair(GREEN_BLACK, CROC_GREEN, CROC_BLACK); 
    init_pair(BLACK_WHITE, CROC_BLACK, CROC_WHITE); 
    init_pair(WHITE_WHITE, CROC_WHITE, CROC_WHITE); 
    init_pair(BLACK_BLACK, CROC_BLACK, CROC_BLACK); 
    init_pair(GREEN_GREEN, CROC_GREEN, CROC_GREEN); 
    init_pair(BLACK_GREEN, CROC_BLACK, CROC_GREEN);
    init_pair(WHITE_GREEN, CROC_WHITE, CROC_GREEN); 
    init_pair(BG_BLACK, COLOR_BLUE, CROC_BLACK); 
    init_pair(BG_GREEN , COLOR_BLACK, COLOR_GREEN);
    //init_pair(BG_BG, 0, 0); 


    /* Frog */
    initColors(FROG_GREEN, 34, 177, 76);
    initColors(FROG_PINK, 255, 163, 177);

    init_pair(GREEN_BLACK_FROG, FROG_GREEN, CROC_BLACK); 
    init_pair(GREEN_GREEN_FROG, FROG_GREEN, FROG_GREEN); 
    init_pair(GREEN_PINK_FROG, FROG_GREEN, FROG_PINK); 
    init_pair(YELLOW_GREEN_FROG, COLOR_YELLOW, COLOR_GREEN);
    init_pair(BLACK_YELLOW_GRASS, CROC_BLACK, COLOR_YELLOW);
    init_pair(BLACK_BLUE, CROC_BLACK, COLOR_BLUE);
    init_pair(BLACK_BLACK_CROC, CROC_BLACK, CROC_BLACK);
    init_pair(BLACK_GREEN_GRASS, CROC_BLACK, COLOR_GREEN);
    init_pair(RED_HEARTS, COLOR_RED, COLOR_BLACK);

    /* Den */
    initColors(LIGHT_GREEN_DEN, 76, 175, 80); 
    initColors(DARK_GREEN_DEN, 27, 94, 32); 

    init_pair(LIGHT_GREEN_DARK_GREEN, LIGHT_GREEN_DEN, DARK_GREEN_DEN);
    init_pair(DARK_GREEN_BLUE, DARK_GREEN_DEN, COLOR_BLUE); 
    init_pair(DARK_GREEN_DARK_GREEN, DARK_GREEN_DEN, DARK_GREEN_DEN);
    init_pair(LIGHT_GREEN_LIGHT_GREEN, LIGHT_GREEN_DEN, LIGHT_GREEN_DEN);

    /* Grenade */
    initColors(DARK_GREY_GRENADE, 66, 66, 66);
    initColors(LIGHT_GREY_GRENADE, 117, 117, 117);
    initColors(MID_GREY_BLACK, 74, 71, 74);
    initColors(BLACK_GRENADE, 33, 33, 33);

    init_pair(LIGHT_DARK_GREY, LIGHT_GREY_GRENADE, DARK_GREY_GRENADE); 
    init_pair(MID_GREY_BLACK, MID_GREY_GRENADE, BLACK_GRENADE);

    /* Projectile */
    initColors(YELLOW_PROJECTILE, 255, 215, 64);
    initColors(LIGHT_ORANGE_PROJECTILE, 255, 171, 0);
    initColors(ORANGE_PROJECTILE, 255, 111, 0);

    init_pair(YELLOW_YELLOW_PROJ, YELLOW_PROJECTILE, YELLOW_PROJECTILE);
    init_pair(LIGHT_LIGHT_PROJ, LIGHT_ORANGE_PROJECTILE, LIGHT_ORANGE_PROJECTILE);
    init_pair(ORANGE_ORANGE_PROJ, ORANGE_PROJECTILE, ORANGE_PROJECTILE);
}