#pragma once
#include "librerie.h"
#include <ncurses.h>

//Moltiplicando un valore RGB compreso tra 0 e 255 per questa costante, si converte il valore nella scala [0 - 1000] utilizzata da ncurses 
#define RGB_CONVERTER 3.921569

void initColors(short c, short r, short g, short b); 
void setColors(); 

// Definizione dei colori utilizzati per i cocodrilli
#define CROC_GREEN 9 
#define CROC_WHITE 10
#define CROC_BLACK 11

// Definizione delle coppie di colori
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

// Definizione dei colori per la rana
#define FROG_GREEN 12
#define FROG_PINK 14

#define GREEN_BLACK_FROG 20
#define GREEN_GREEN_FROG 21
#define GREEN_PINK_FROG 22
#define YELLOW_GREEN_FROG 23
#define BLACK_YELLOW_GRASS 24
#define BLACK_BLUE 25
#define BLACK_BLACK_CROC 26
#define BLACK_GREEN_GRASS 27

// Definizione dei colori per le vite
#define RED_HEARTS 29

// Definizione dei colori per le tane
#define LIGHT_GREEN_DEN 15
#define DARK_GREEN_DEN 16

#define LIGHT_GREEN_DARK_GREEN 30 
#define DARK_GREEN_BLUE 31
#define DARK_GREEN_DARK_GREEN 32
#define LIGHT_GREEN_LIGHT_GREEN 33

// Definizione dei colori per le granate
#define DARK_GREY_GRENADE 17
#define LIGHT_GREY_GRENADE 18
#define MID_GREY_GRENADE 19
#define BLACK_GRENADE 20

#define LIGHT_DARK_GREY 34
#define MID_GREY_BLACK 35

// Definizione dei colori per i proiettili
#define YELLOW_PROJECTILE 21
#define LIGHT_ORANGE_PROJECTILE 22
#define ORANGE_PROJECTILE 23

#define YELLOW_YELLOW_PROJ 36
#define LIGHT_LIGHT_PROJ 37
#define ORANGE_ORANGE_PROJ 38

