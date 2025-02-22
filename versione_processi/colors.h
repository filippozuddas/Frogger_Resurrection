#pragma once
#include <ncurses.h>

//Moltiplicando un valore RGB compreso tra 0 e 255 per questa costante, si converte il valore nella scala [0 - 1000] utilizzata da ncurses 
#define RGB_CONVERTER 3.921569

void initColors(short c, short r, short g, short b); 
void setColors(); 

//croc

#define CROC_GREEN 9 
#define CROC_WHITE 10
#define CROC_BLACK 11

//Coppie di colori 

#define BLACK_BG 10
#define GREEN_BLACK 11
#define BLACK_WHITE 12
#define WHITE_WHITE 13
#define BLACK_BLACK 14
#define GREEN_GREEN 15
#define BLACK_GREEN 16
#define WHITE_GREEN 17
#define BG_BLACK 18
#define BG_BG 19
#define BG_GREEN 28

/* Frog */

#define FROG_GREEN 12
#define FROG_PINK 14

#define GREEN_BLACK_FROG 20
#define GREEN_GREEN_FROG 21
#define GREEN_PINK_FROG 22
#define YELLOW_GREEN_FROG 23
#define BLACK_GREEN_GRASS 24
#define BLACK_BLUE 25
#define BLACK_BLACK_CROC 26
#define BLACK_MAGENTA 27

/*vite*/
#define RED_HEARTS 29
