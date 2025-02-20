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
void inputHandler(Game *game); 
int checkCollision(Informations frogInfo, Informations crocInfo);
int isFrogOnCroc(Game *game);
int isFrogOnRiver(Game *game);
int isFrogOnDen(Game *game);
int isFrogOnTopBank(Game *game);
void createGrenade(Game *game, int direction, int grenadeId, int grenadeIndex); 
void moveGrenade(Grenade *grenade, Game *game, int grenadeIndex); 
void terminateGrenades(Game *game);
int checkCollisionProjectile(Informations entity, Projectile projectile);