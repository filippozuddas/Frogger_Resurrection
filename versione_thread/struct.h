#pragma once

#include <pthread.h>
#include <ncursesw/ncurses.h>
#include "globali.h"
#include <wchar.h>



// Struttura per le informazioni condivise tra le entità del gioco
typedef struct {
    int x; 
    int y; 
    int direction; 
    int speed; 
    int ID;   // 0 = frog, 1 = croc
    int active;
    int grenadesRemaining; 
} Informations;

// Struttura per i coccodrilli
typedef struct Crocodile{
    Informations info;
    pthread_t thread;
} Crocodile;


// Structura per la rana
typedef struct Frog {
    Informations info;
    int lives; 
    int score; 
    int isOnCroc; 
    int onCrocIdx;
    int onCrocOffset; 
    pthread_t thread;
} Frog; 


// Struttura per le granate 
typedef struct Grenade {
    Informations info; 
    pthread_t thread;
} Grenade;


// Struttura per i proiettili
typedef struct Projectile {
    Informations info;
    pthread_t thread;
} Projectile;


// Struttura per le tane
typedef struct Den {
    int x; 
    int y; 
    int width; 
    int height;
    int isOpen; 
} Den;


// Struttura per le opzioni del menu
typedef struct {
    int id;
    const wchar_t *text[5]; 
} MenuOption;


// Struttura per i nodi della lista dei punteggi
typedef struct ScoreNode {
    int score;
    struct ScoreNode* next;
} ScoreNode;

// Struttura per le voci della tabella dei punteggi
typedef struct ScoreEntry{
    int score;
    char date[20];
    int difficulty;  
} ScoreEntry;


// Struttura principale del gioco
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