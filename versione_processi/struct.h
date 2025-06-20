#pragma once

#include <sys/types.h>
#include <wchar.h>
#include <ncurses.h>

#define N_FLOW 13
#define CROC_PER_FLOW 2
#define N_CROC (N_FLOW * CROC_PER_FLOW)
#define CROC_HEIGHT 4
#define CROC_LENGHT 21
#define MIN_CROC_DISTANCE 20

#define FROG_WIDTH 10
#define FROG_HEIGHT 4

#define MAX_GRENADES 10
#define MAX_PROJECTILES 30

#define N_DENS 5

// Dimensioni dello schermo
#define GAME_WIDTH 200
#define GAME_HEIGHT 70

#define MAX_HEIGHT 6
#define PATTERN_WIDTH 148
#define MAX_HEIGHT_WELCOME 26


#define TIMEOUT_MS 1000

#define MENU_ITEMS 4
#define LEVEL_ITEMS 3
#define RESTART_ITEMS 2

#define MAX_SCORES 10
#define SCORES_FILE "scores.dat"

#define SCORE_X 100  // Regola la posizione orizzontale
#define SCORE_Y 5    // Regola la posizione verticale

#define DIGIT_HEIGHT 3

/* Struttura base per le informazioni comuni di oggetti di gioco */
typedef struct {
    int x;                  // Posizione X 
    int y;                  // Posizione Y
    int direction;          // Direzione di movimento
    int speed;              // Velocità di movimento
    int ID;                 // Identificatore univoco
    int grenadesRemaining;  // Numero di granate rimanenti
    pid_t pid;              // PID del processo
} Informations;

/* Struttura per i coccodrilli */
typedef struct Crocodile{
    Informations info;      // Informazioni base del coccodrillo
    int mainToCrocPipe[2];  // Pipe inversa per comunicazione processo principale-coccodrillo
} Crocodile;

/* Struttura per la rana (giocatore) */
typedef struct Frog {
    Informations info;      // Informazioni base della rana
    int lives;              // Vite rimanenti
    int score;              // Punteggio attuale
    int isOnCroc;           // Flag: 1 se la rana è su un coccodrillo, 0 altrimenti
    int onCrocIdx;          // Indice del coccodrillo su cui si trova la rana
    int onCrocOffset;       // Offset della posizione della rana sul coccodrillo
} Frog; 

/* Struttura per le granate */
typedef struct Grenade {
    Informations info;      // Informazioni base della granata
} Grenade;

/* Struttura per i proiettili */
typedef struct Projectile {
    Informations info;      // Informazioni base del proiettile
} Projectile;

/* Struttura per le tane */
typedef struct Den {
    int x;                  // Posizione X della tana
    int y;                  // Posizione Y della tana
    int width;              // Larghezza della tana
    int height;             // Altezza della tana
    int isOpen;             // Flag: 1 se la tana è aperta, 0 altrimenti
} Den;

/* Struttura principale del gioco */
typedef struct Game{
    unsigned int isRunning; // Flag: 1 se il gioco è in esecuzione, 0 altrimenti
    int pipeFd[2];          // Pipe principale
    Frog frog;              // Istanza della rana (giocatore)
    Crocodile crocodile[N_CROC]; // Array dei coccodrilli
    Grenade grenades[MAX_GRENADES]; // Array delle granate
    Projectile projectiles[MAX_PROJECTILES]; // Array dei proiettili
    Den dens[N_DENS];       // Array delle tane
    WINDOW *gameWin;        // Finestra del gioco 
    int difficulty;         // Livello di difficoltà
} Game;

/* Struttura per le opzioni di menu */
typedef struct {
    int id;                 // Identificatore dell'opzione
    const wchar_t *text[5]; // Array di stringhe per ogni opzione
} MenuOption;

/* Struttura per un nodo nella lista dei punteggi */
typedef struct ScoreNode {
    int score;              // Punteggio
    struct ScoreNode* next; // Puntatore al prossimo nodo
} ScoreNode;

/* Struttura per un'entrata nel punteggio */
typedef struct {
    int score;              // Punteggio
    char date[20];          // Data del punteggio
    int difficulty;         // Difficoltà: 0=Facile, 1=Medio, 2=Difficile
} ScoreEntry;
