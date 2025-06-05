#include "thread.h"  // Include the corresponding header FIRST

pthread_mutex_t buffer_mutex;  // mutex condiviso per sezione critica, accesso al buffer
pthread_mutex_t buffer2_mutex; 
pthread_mutex_t ncurses_mutex; // mutex per gestire le funzioni bloccanti di ncurses

pthread_mutex_t projectile_mutex;  // mutex usato dai proiettili per scrivere nel buffer le loro coordinate
pthread_mutex_t granades_mutex; // mutex usato dalle granate per scrivere nel buffer le loro coordinate


// main buffer, usato tra i produttori e il consumatore
Informations buffer[DIM_BUFFER];
sem_t sem_free; //semaforo per gestione posti liberi nel buffer
sem_t sem_occupied; // semaforo per gestione posti occupati nel buffer
int index_write; //head 
int index_read; //tail

// buffer secondario dal main ai produttori croc e frog
Informations buffer2[DIM_BUFFER];
sem_t sem_free2;
sem_t sem_occupied2;
int index_write2;
int index_read2 ;


//usata dai produttori per scrivere nel buffer usato anche dal consumatore e dagli altri produttori
void writeMain(Informations info) {
    sem_wait(&sem_free);  
    pthread_mutex_lock(&buffer_mutex);
    buffer[index_write] = info;
    index_write = (index_write + 1) % DIM_BUFFER;
    pthread_mutex_unlock(&buffer_mutex);
    sem_post(&sem_occupied); 
}

//usata per leggere dal buffer , usata solo dal consumatore che deve identificare da dove arriva il messaggio
Informations readMain() {
    Informations info;
    if (sem_trywait(&sem_occupied) == 0) { 
        pthread_mutex_lock(&buffer_mutex);
        info = buffer[index_read];
        index_read = (index_read + 1) % DIM_BUFFER;
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&sem_free);
        return info;
    }
}

//usata dal consumatore per scrivere nel buffer condiviso verso i produttori, dal quale frog e croc leggono
void writeProd(Informations oggetto) {
    sem_wait(&sem_free2);  // Wait for space in croc buffer
    pthread_mutex_lock(&buffer_mutex);
    buffer2[index_write2] = oggetto;
    index_write2 = (index_write2 + 1) % DIM_BUFFER2;
    pthread_mutex_unlock(&buffer_mutex);
    sem_post(&sem_occupied2); // Signal croc buffer has data
}

Informations readProd() {
    Informations info;
    if (sem_trywait(&sem_occupied2) == 0) { 
        pthread_mutex_lock(&buffer_mutex);
        info = buffer2[index_read2];
        index_read2 = (index_read2 + 1) % DIM_BUFFER;
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&sem_free2);
        return info;
    }
}

void resetBuffer() {
    // Reset main buffer
    pthread_mutex_lock(&buffer_mutex);
    index_write = 0;
    index_read = 0;
    while (sem_trywait(&sem_occupied) == 0); // Empty the buffer
    for (int i = 0; i < DIM_BUFFER; ++i) {
        sem_post(&sem_free); // Fill with free slots
    }

    // Reset buffer2
    index_write2 = 0;
    index_read2 = 0;
    while (sem_trywait(&sem_occupied2) == 0);
    for (int i = 0; i < DIM_BUFFER2; ++i) {
        sem_post(&sem_free2);
    }
    pthread_mutex_unlock(&buffer_mutex);
}

void init_synchro() {
    // Initialize main buffer semaphores
    sem_init(&sem_occupied, 0, 0);
    sem_init(&sem_free, 0, DIM_BUFFER);

    index_write = 0; 
    index_read = 0; 
    index_write2 = 0; 
    index_read2 = 0; 


    // Initialize crocodile buffer semaphores
    sem_init(&sem_occupied2, 0, 0);
    sem_init(&sem_free2, 0, DIM_BUFFER2);

    pthread_mutex_init(&buffer_mutex, NULL);
    pthread_mutex_init(&buffer2_mutex, NULL);
    pthread_mutex_init(&ncurses_mutex, NULL);
    pthread_mutex_init(&projectile_mutex, NULL);
    pthread_mutex_init(&granades_mutex, NULL);
}

void deallocate_synchro() {
    // Destroy main buffer semaphores
    sem_destroy(&sem_occupied);
    sem_destroy(&sem_free);

    // Destroy crocodile buffer semaphores
    sem_destroy(&sem_occupied2);
    sem_destroy(&sem_free2);

    pthread_mutex_destroy(&buffer_mutex);
    pthread_mutex_destroy(&buffer2_mutex);
    pthread_mutex_destroy(&ncurses_mutex);
    pthread_mutex_destroy(&projectile_mutex);
    pthread_mutex_destroy(&granades_mutex);
}

void wait_prod() {
    sem_wait(&sem_free); // Assuming prod_sem is a globally declared semaphore
}

void signal_prod() {
    sem_post(&sem_occupied); // Assuming prod_sem is a globally declared semaphore
}

void wait_cons() {
    sem_wait(&sem_occupied); // Assuming cons_sem is a globally declared semaphore
}

void signal_cons() {
    sem_post(&sem_free); // Assuming cons_sem is a globally declared semaphore
}