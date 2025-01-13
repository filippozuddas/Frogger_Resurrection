#pragma once

#include "colors.h"
#include "crocodile.h"
#include "frog.h"
#include "map.h"
#include "sprite.h"
#include "colors.h"

typedef struct {
    unsigned int isRunning; 
    int pipeFd[2]; 
    Crocodile crocodile[N_CROC]; 
    Frog frog; 
} Game;

void initGame(Game *game); 
void runGame(Game *game); 
void stopGame(Game *game); 