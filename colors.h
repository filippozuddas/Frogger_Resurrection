#pragma once
#include <ncurses.h>

//Moltiplicando un valore RGB compreso tra 0 e 255 per questa costante, si converte il valore nella scala [0 - 1000] utilizzata da ncurses 
#define RGB_CONVERTER 3.921569

#define CHANGE_COLOR(n) attron(COLOR_PAIR(n))

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
