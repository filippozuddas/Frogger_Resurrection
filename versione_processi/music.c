#include "music.h"

// Funzione per inizializzare l'audio con SDL e SDL_mixer
int initAudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Errore inizializzazione SDL: %s\n", SDL_GetError());
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Errore inizializzazione SDL_mixer: %s\n", Mix_GetError());
        SDL_Quit(); // De-inizializza SDL se l'inizializzazione di SDL_mixer fallisce.
        return 0;
    }
    return 1;
}

// Funzione per caricare e riprodurre un file audio
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

// Funzione per fermare la musica in riproduzione
void stopMusic() {
    Mix_HaltMusic();
}

// Funzione per terminare l'audio e de-inizializzare SDL
void terminateAudio() {
    Mix_CloseAudio();
    SDL_Quit();
}