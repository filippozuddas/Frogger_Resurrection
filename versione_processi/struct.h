#pragma once

#include <sys/types.h>
#include <wchar.h>
#include <ncurses.h>

#define N_FLOW 13
#define CROC_PER_FLOW 2
#define N_CROC (N_FLOW * CROC_PER_FLOW)
#define CROC_HEIGHT 4
#define CROC_LENGHT 21
#define MIN_CROC_DISTANCE 20

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

typedef struct {
    int x; 
    int y; 
    int direction; 
    int speed; 
    int ID;   // 0 = frog, 1 = croc
    int grenadesRemaining; 
    pid_t pid;
} Informations;

typedef struct Crocodile{
    Informations info;
    int mainToCrocPipe[2];
} Crocodile;

typedef struct Frog {
    Informations info;
    int lives; 
    int score; 
    int isOnCroc; 
    int onCrocIdx;
    int onCrocOffset; 
} Frog; 

typedef struct Grenade {
    Informations info; 
} Grenade;

typedef struct Projectile {
    Informations info;
} Projectile;

typedef struct Den {
    int x; 
    int y; 
    int width; 
    int height;
    int isOpen; 
} Den;

typedef struct Game{
    unsigned int isRunning; 
    int pipeFd[2]; 
    int mainToFrogPipe[2];
    Frog frog; 
    Crocodile crocodile[N_CROC]; 
    Grenade grenades[MAX_GRENADES];
    Projectile projectiles[MAX_PROJECTILES];
    Den dens[N_DENS];
    WINDOW *gameWin;
    int difficulty;
} Game;

typedef struct {
    int id;
    const wchar_t *text[5]; // Array di stringhe per ogni opzione
} MenuOption;

typedef struct ScoreNode {
    int score;
    struct ScoreNode* next;
} ScoreNode;

typedef struct {
    int score;
    char date[20];
    int difficulty;  // 0=Easy, 1=Medium, 2=Hard, 3=Expert
} ScoreEntry;
