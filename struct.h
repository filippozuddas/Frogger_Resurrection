#pragma once

#define MAX_V 8
#define MIN_V 5
#define N_FLOW 8
#define CROC_PER_FLOW 2
#define N_CROC (N_FLOW * CROC_PER_FLOW)
#define CROC_HEIGHT 4
#define CROC_LENGHT 21
#define MIN_CROC_DISTANCE 8

typedef enum {
    MSG_FROG,
    MSG_CROC
} MessageType;

typedef struct {
    int x; 
    int y; 
    int direction; 
    int speed; 
    int ID;   // 0 = frog, 1 = croc
    pid_t pid;
} Informations;

typedef struct {
    Informations info;
} Crocodile;

typedef struct Frog {
    Informations info;
    int lives; 
    int score; 
    int isOnCroc; 
}Frog; 

typedef struct {
    unsigned int isRunning; 
    int pipeFd[2]; 
    Crocodile crocodile[N_CROC]; 
    Frog frog; 
} Game;