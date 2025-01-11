#pragma once
#include "game.h"


#define MAX_V 5
#define MIN_V 3
#define N_FLUSSI 8
#define CROC_PER_ROW 4
#define NUM_COCCODRILLI (N_FLUSSI * CROC_PER_ROW)
#define CROC_HEIGHT 3

//struttura per il un singolo coccodrillo 
typedef struct {
    int x;
    int y;
    int isVisible;
    pid_t pid;
} Coccodrillo;

//array globale per la velocità di ogni flusso
extern int rowSpeed[N_FLUSSI];

//array globale per la direzione di ogni flusso 
extern int rowDirection[N_FLUSSI]; 

const char *crocSprite[CROC_HEIGHT] = {
    "|=======|",
    "|=======|",
    "|=======|"
};

//funzione per la gestione della logica e dei movimenti del coccodrillo 
void fun_coccodrillo(int pipe_scrittura, int crod_id); 
void initSpeed();
void initDirection();
void createCroc(int pipeFd[2]);
void printCroc(char *crocSprite, Coccodrillo *croc);
void readCroc();