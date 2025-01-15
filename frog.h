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


#define FROG_LENGTH 7
#define FROG_HEIGHT 4

void createFrog(Frog *frog, int pipeFd[]);
void moveFrog(Frog *frog, int pipeFd[]); 