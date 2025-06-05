#pragma once

#include "librerie.h"

extern const char *crocSprite[CROC_HEIGHT][CROC_LENGHT];
extern int flowDirection[N_FLOW]; 
extern int flowSpeed[N_FLOW]; 

void createCroc(Game *game); 
void* moveCroc(void* arg);
void resetCroc(Game *game);
void killCroc(Game *game); 
void createProjectile(Game *game, Crocodile *croc, int projectileID, int projectileIndex);
void* moveProjectile(void* arg);
void handleProjectileGeneration(Game *game);
void terminateProjectiles(Game *game); 
int isPositionValid(int x_new, int y_new, Crocodile *crocodiles, int count);
