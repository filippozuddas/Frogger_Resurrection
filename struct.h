#pragma once

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