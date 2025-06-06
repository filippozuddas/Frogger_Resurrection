#pragma once

//Moltiplicando un valore RGB compreso tra 0 e 255 per questa costante, si converte il valore nella scala [0 - 1000] utilizzata da ncurses 
#define RGB_CONVERTER 3.921569

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define N_FLOW 8
#define CROC_PER_FLOW 2
#define N_CROC (N_FLOW * CROC_PER_FLOW)
#define CROC_HEIGHT 4
#define CROC_LENGHT 21
#define MIN_CROC_DISTANCE 10

#define FROG_WIDTH 10
#define FROG_HEIGHT 4

#define MAX_GRENADES 10
#define MAX_PROJECTILES 30

#define N_DENS 5

// Dimensioni dello schermo
#define GAME_WIDTH 200
#define GAME_HEIGHT 70

#define MAX_HEIGHT 6
#define PATTERN_WIDTH 148
#define MAX_HEIGHT_WELCOME 26


#define TIMEOUT_MS 1000

#define MENU_ITEMS 4
#define LEVEL_ITEMS 3
#define RESTART_ITEMS 2

#define MAX_SCORES 10
#define SCORES_FILE "scores.dat"

#define SCORE_X 100  // Regola la posizione orizzontale
#define SCORE_Y 5    // Regola la posizione verticale

#define DIGIT_HEIGHT 3

#define DIM_BUFFER 300
#define DIM_BUFFER2 300