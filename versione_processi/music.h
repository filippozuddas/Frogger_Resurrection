#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

int initAudio();
void startMusic(const char* file);
void stopMusic();
void terminateAudio();