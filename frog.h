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

void createFrog(Frog *frog, int *pipeFd, int *mainToEntPipe);
void inputHandler(Frog *frog, int *pipeFd, int *mainToEntPipe); 
int checkCollision(Informations frogInfo, Informations crocInfo);
int isFrogOnCroc(Frog *frog, Crocodile *croc);