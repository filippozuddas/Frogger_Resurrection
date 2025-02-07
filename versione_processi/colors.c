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
    //init_pair(BG_BG, 0, 0); 


    /* Frog */
    initColors(FROG_GREEN, 34, 177, 76);
    initColors(FROG_PINK, 255, 163, 177);

    init_pair(GREEN_BLACK_FROG, FROG_GREEN, CROC_BLACK); 
    init_pair(GREEN_GREEN_FROG, FROG_GREEN, FROG_GREEN); 
    init_pair(GREEN_PINK_FROG, FROG_GREEN, FROG_PINK); 
    init_pair(YELLOW_GREEN_FROG, COLOR_YELLOW, COLOR_GREEN);
    init_pair(BLACK_GREEN_GRASS, CROC_BLACK, COLOR_GREEN);
    init_pair(BLACK_BLUE, CROC_BLACK, COLOR_BLUE);
    init_pair(BLACK_BLACK_CROC, CROC_BLACK, CROC_BLACK);
    init_pair(BLACK_MAGENTA, CROC_BLACK, COLOR_MAGENTA);
    
}