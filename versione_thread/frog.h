#pragma once 

#include "struct.h"
#include <ncursesw/ncurses.h>


void createFrog(Game *game);
void *inputHandler(void* params);
void createGrenade(Game *game, int direction);
void moveGrenade(void * params);
void terminateGrenades(Game *game);
int findFreeGrenadeSlot(Game *game);
int isFrogOnCroc(Game *game);
int isFrogOnRiver(Game *game);
int isFrogOnDen(Game *game);
int isFrogOnTopBank(Game *game);
int checkCollisionProjectile(Informations entity, Projectile projectile);
int checkCollision(Informations frogInfo, Informations crocInfo);
