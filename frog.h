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

void createFrog(Frog *frog, int *pipeFd, int *mainToFrogPipe);
void inputHandler(Frog *frog, int *pipeFd, int *mainToFrogPipe); 
int checkCollision(Informations frogInfo, Informations crocInfo);
int isFrogOnCroc(Game *game);
int isFrogOnRiver(Game *game);
void createGrenade(Frog *frog, int *pipeFd, int direction); 
void moveGrenade(Grenade *grenade, int *pipeFd); 