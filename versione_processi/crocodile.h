#pragma once

#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "struct.h"

extern const char *crocSprite[CROC_HEIGHT][CROC_LENGHT];
extern int flowDirection[N_FLOW]; 
extern int flowSpeed[N_FLOW]; 

void createCroc(Game *game); 
void moveCroc(Crocodile *croc, int *pipeFd); 
void resetCroc(Game *game);
void killCroc(Game *game); 
void createProjectile(Crocodile *croc, int *pipeFd, int projectileID); 
void moveProjectile(Projectile *projectile, int *pipeFd); 
void handleProjectileGeneration(Game *game);
