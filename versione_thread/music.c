#include "music.h"

// Inizializza SDL e SDL_mixer per la gestione dell'audio
int initAudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Errore inizializzazione SDL: %s\n", SDL_GetError());
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Errore inizializzazione SDL_mixer: %s\n", Mix_GetError());
        SDL_Quit(); 
        return 0;
    }
    return 1;
}

// Funzioni per gestire la musica
void startMusic(const char* file) {
    Mix_Music *music = Mix_LoadMUS(file);
    if (!music) {
        printf("Errore caricamento musica: %s\n", Mix_GetError());
        return;
    }
    if (Mix_PlayMusic(music, -1) == -1) {
        printf("Errore riproduzione musica: %s\n", Mix_GetError());
        Mix_FreeMusic(music);
        return;
    }
}

void stopMusic() {
    Mix_HaltMusic();
}

void terminateAudio() {
    Mix_CloseAudio();
    SDL_Quit();
}