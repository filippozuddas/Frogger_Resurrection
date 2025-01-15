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

#define MAX_V 8
#define MIN_V 5
#define N_FLOW 8
#define CROC_PER_FLOW 2
#define N_CROC (N_FLOW * CROC_PER_FLOW)
#define CROC_HEIGHT 4
#define CROC_LENGHT 21
#define MIN_CROC_DISTANCE 8

extern const char *crocSprite[CROC_HEIGHT][CROC_LENGHT];
extern int flowDirection[N_FLOW]; 
extern int flowSpeed[N_FLOW]; 

void createCroc(Crocodile *croc, int *pipeFd); 
void moveCroc(Crocodile *croc, int *pipeFd); 