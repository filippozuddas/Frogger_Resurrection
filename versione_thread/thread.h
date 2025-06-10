#pragma once

#include "librerie.h" // Include librerie.h for Informations, etc.


// Declare EXTERNAL variables (don't define them here)
extern pthread_mutex_t buffer_mutex;
extern pthread_mutex_t buffer2_mutex;
extern pthread_mutex_t ncurses_mutex;
extern pthread_mutex_t projectile_mutex; 
extern pthread_mutex_t granades_mutex;

// Main buffer
extern Informations buffer[DIM_BUFFER];
extern sem_t sem_free;
extern sem_t sem_occupied;
extern int index_write;
extern int index_read;

// Crocodile buffer
extern Informations buffer2[DIM_BUFFER];
extern sem_t sem_free2;
extern sem_t sem_occupied2;
extern int index_write2;
extern int index_read2;


// Function prototypes (Keep them in librerie.h, this is just for clarity.  Having them in both is fine.)
void writeMain(Informations info);
bool readMain(Informations* info);
void writeProd(Informations oggetto);
Informations readProd();


void resetBuffer();  // Good to have a reset function
void init_synchro();
void deallocate_synchro();
void wait_prod();       // Correct prototypes with sem_t*
void signal_prod();
void wait_cons();
void signal_cons();

