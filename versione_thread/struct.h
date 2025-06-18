#pragma once

//#include "librerie.h"

#include <pthread.h>
#include <ncursesw/ncurses.h>
#include "globali.h"
#include <wchar.h>



typedef struct {
    int x; 
    int y; 
    int direction; 
    int speed; 
    int ID;   // 0 = frog, 1 = croc
    int active;
    int grenadesRemaining; 
} Informations;

typedef struct Crocodile{
    Informations info;
    pthread_t thread;
} Crocodile;

typedef struct Frog {
    Informations info;
    int lives; 
    int score; 
    int isOnCroc; 
    int onCrocIdx;
    int onCrocOffset; 
    pthread_t thread;
} Frog; 

typedef struct Grenade {
    Informations info; 
    pthread_t thread;
} Grenade;

typedef struct Projectile {
    Informations info;
    pthread_t thread;
} Projectile;

typedef struct Den {
    int x; 
    int y; 
    int width; 
    int height;
    int isOpen; 
} Den;


typedef struct {
    int id;
    const wchar_t *text[5]; // Array di stringhe per ogni opzione
} MenuOption;

typedef struct ScoreNode {
    int score;
    struct ScoreNode* next;
} ScoreNode;

typedef struct ScoreEntry{
    int score;
    char date[20];
    int difficulty;  // 0=Easy, 1=Medium, 2=Hard, 3=Expert
} ScoreEntry;

typedef struct Game{
    unsigned int isRunning; 
    Frog frog; 
    Crocodile crocodile[N_CROC]; 
    Grenade grenades[MAX_GRENADES];
    Projectile projectiles[MAX_PROJECTILES];
    Den dens[N_DENS];
    WINDOW *gameWin;
    int difficulty;
} Game;