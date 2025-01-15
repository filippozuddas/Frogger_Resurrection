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
} Coordinates;

typedef struct {
    pid_t pid;
    Coordinates coords;
} Crocodile;

typedef struct Frog {
    Coordinates coords;
    int lives; 
    int score; 
    int isOnCroc; 
}Frog; 