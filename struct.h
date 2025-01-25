#pragma once

#define MAX_V 12
#define MIN_V 8
#define N_FLOW 8
#define CROC_PER_FLOW 2
#define N_CROC (N_FLOW * CROC_PER_FLOW)
#define CROC_HEIGHT 4
#define CROC_LENGHT 21
#define MIN_CROC_DISTANCE 8

#define FROG_LENGTH 10
#define FROG_HEIGHT 4

#define MAX_GRENADES 10

typedef struct {
    int x; 
    int y; 
    int direction; 
    int speed; 
    int ID;   // 0 = frog, 1 = croc
    pid_t pid;
} Informations;

typedef struct Crocodile{
    Informations info;
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

typedef struct Game{
    unsigned int isRunning; 
    int pipeFd[2]; 
    int mainToCrocPipe[2];
    int mainToFrogPipe[2];
    Frog frog; 
    Crocodile crocodile[N_CROC]; 
    Grenade grenades[MAX_GRENADES];
} Game;