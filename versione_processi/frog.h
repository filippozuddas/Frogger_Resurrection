#pragma once 

#include <ncurses.h>
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

void createFrog(Game *game);
void inputHandler(Game *game, Frog *frog); 
int checkCollision(Informations frogInfo, Informations crocInfo);
int isFrogOnCroc(Game *game);
int isFrogOnRiver(Game *game);
int isFrogOnDen(Game *game);
void createGrenade(Game *game, Frog *frog, int direction, int grenadeId); 
void moveGrenade(Grenade *grenade, int *pipeFd); 
