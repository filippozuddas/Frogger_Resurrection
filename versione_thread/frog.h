#pragma once 

#include "librerie.h"


void createFrog(Game *game);
void createGrenade(Game *game, int direction, int grenadeId, int grenadeIndex);
void moveGrenade(void * params);
void terminateGrenades(Game *game);
int findFreeGrenadeSlot(Game *game);
int isFrogOnCroc(Game *game);
int isFrogOnRiver(Game *game);
int isFrogOnDen(Game *game);
int isFrogOnTopRiver(Game *game);
int isFrogOnTopBank(Game *game);
int checkCollisionProjectile(Informations entity, Projectile projectile);
int checkCollision(Informations frogInfo, Informations crocInfo);
