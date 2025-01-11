#include "colors.h"

void initColors(short c, short r, short g, short b) {
    init_color(c, r * RGB_CONVERTER, g * RGB_CONVERTER, b * RGB_CONVERTER);  
}

void setColors() {

    initColors(CROC_GREEN, 173,255,47);
    initColors(CROC_WHITE, 255,250,250); 
    initColors(CROC_BLACK, 0, 0, 0); 

    init_pair(BLACK_BG, CROC_BLACK, 0); 
    init_pair(GREEN_BLACK, CROC_GREEN, CROC_BLACK); 
    init_pair(BLACK_WHITE, CROC_BLACK, CROC_WHITE); 
    init_pair(WHITE_WHITE, CROC_WHITE, CROC_WHITE); 
    init_pair(BLACK_BLACK, CROC_BLACK, CROC_BLACK); 
    init_pair(GREEN_GREEN, CROC_GREEN, CROC_GREEN); 
    init_pair(BLACK_GREEN, CROC_BLACK, CROC_GREEN);
    init_pair(WHITE_GREEN, CROC_WHITE, CROC_GREEN); 
    init_pair(BG_BLACK, 0, CROC_BLACK); 
    init_pair(BG_BG, 0, 0); 
}