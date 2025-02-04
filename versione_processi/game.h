#pragma once

#include "colors.h"
#include "crocodile.h"
#include "frog.h"
#include "map.h"
#include "sprite.h"
#include "colors.h"
#include "struct.h"
#include "map.h"

void initDens(Game *game);
void initGame(Game *game); 
void runGame(Game *game); 
void stopGame(Game *game); 
ssize_t full_read(int fd, void *buffer, size_t size);
void flushPipe(int fd);
void setNonBlocking(int fd);