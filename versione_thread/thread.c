#include "thread.h"  

pthread_mutex_t buffer_mutex;  // mutex condiviso per sezione critica, accesso al buffer
pthread_mutex_t ncurses_mutex; // mutex per gestire le funzioni bloccanti di ncurses


// main buffer, usato tra i produttori e il consumatore
Informations buffer[DIM_BUFFER];
sem_t sem_free; //semaforo per gestione posti liberi nel buffer
sem_t sem_occupied; // semaforo per gestione posti occupati nel buffer
int index_write; 
int index_read; 


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
bool readMain(Informations* info) {
    // Tenta di "prendere" un semaforo occupato in modo non bloccante.
    // Se sem_trywait restituisce 0, significa che c'era un elemento e l'abbiamo preso.
    if (sem_trywait(&sem_occupied) == 0) {
        
        // Se siamo qui, il buffer ha almeno un elemento valido.
        pthread_mutex_lock(&buffer_mutex);
        *info = buffer[index_read]; // Copia l'elemento nel puntatore che ci è stato passato
        index_read = (index_read + 1) % DIM_BUFFER;
        pthread_mutex_unlock(&buffer_mutex);

        sem_post(&sem_free); // Segnala che si è liberato un posto nel buffer.
        return true;         // Ritorna 'true' per indicare che la lettura è andata a buon fine.
    }

    // Se sem_trywait non ha restituito 0, significa che il buffer era vuoto.
    return false; // Ritorna 'false' per indicare che non abbiamo letto nulla.
}


void resetBuffer() {
    pthread_mutex_lock(&buffer_mutex);
    
    // Reset solo per il buffer principale
    index_write = 0;
    index_read = 0;
    
    // Svuota il semaforo degli elementi occupati
    while (sem_trywait(&sem_occupied) == 0) {
        // Continua a consumare finché il semaforo è vuoto
    }
    
    // Ripristina il semaforo dei posti liberi
    // (Questo assicura che il produttore non si blocchi)
    for (int i = 0; i < DIM_BUFFER; ++i) {
        sem_post(&sem_free);
    }
    
    pthread_mutex_unlock(&buffer_mutex);
}

// Inizializzazione della sincronizzazione
void init_synchro() {
    sem_init(&sem_occupied, 0, 0);
    sem_init(&sem_free, 0, DIM_BUFFER);

    index_write = 0; 
    index_read = 0; 

    pthread_mutex_init(&buffer_mutex, NULL);
    pthread_mutex_init(&ncurses_mutex, NULL);

}

// Deallocazione della sincronizzazione
void deallocate_synchro() {
    sem_destroy(&sem_occupied);
    sem_destroy(&sem_free);

    pthread_mutex_destroy(&buffer_mutex);
    pthread_mutex_destroy(&ncurses_mutex);

}

void wait_prod() {
    sem_wait(&sem_free); 
}

void signal_prod() {
    sem_post(&sem_occupied); 
}

void wait_cons() {
    sem_wait(&sem_occupied); 
}

void signal_cons() {
    sem_post(&sem_free);
}