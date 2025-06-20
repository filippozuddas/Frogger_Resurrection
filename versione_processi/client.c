#include "socket.h"
#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <locale.h>

/*
 * Gestisce il loop principale del client, leggendo l'input dell'utente,
 * comunicando con il server e aggiornando lo stato del gioco.
 */
static void runInputClient(int client_socket_fd) {
    // Imposta getch() come non bloccante
    nodelay(stdscr, TRUE);
    // Abilita la lettura di tasti speciali (es. frecce)
    keypad(stdscr, TRUE);

    // Struttura per memorizzare le informazioni correnti ricevute dal server
    Informations current_msg;
    current_msg.x = 0;
    current_msg.y = 0;
    current_msg.grenadesRemaining = 0;

    // Struttura per preparare il nuovo messaggio da inviare al server    
    Informations new_msg;

    // Loop principale del client    
    while (true) {
        // Riceve lo stato aggiornato del gioco dal server
        ssize_t recv_result = receiveInfo(client_socket_fd, &current_msg);
        
        // Verifica se il server ha inviato un segnale di terminazione
        if (recv_result > 0 && current_msg.ID == -99) {
            // Ricevuto segnale di terminazione dal server
            break;
        }

        // Legge l'input dell'utente
        int input = getch();

        bool has_input = true;
        new_msg = current_msg;

        // Gestisce l'input dell'utente
        switch (input) {
            case 'w': 
            case 'W': 
            case KEY_UP:
                new_msg.y = (current_msg.y > 0) ? current_msg.y - FROG_HEIGHT : current_msg.y;
                break;
            case 's': 
            case 'S': 
            case KEY_DOWN:
                new_msg.y = (current_msg.y < GAME_HEIGHT - FROG_HEIGHT - 1) ? current_msg.y + FROG_HEIGHT : current_msg.y;
                break;
            
            case 'a': 
            case 'A': 
            case KEY_LEFT:
                new_msg.x = (current_msg.x > 0) ? current_msg.x - 1 : current_msg.x;
                break;
            
            case 'd': 
            case 'D': 
            case KEY_RIGHT:
                new_msg.x = (current_msg.x < GAME_WIDTH - FROG_WIDTH) ? current_msg.x + 1 : current_msg.x;
                break;
            case ' ':    // Tasto "spazio" per lanciare la granata
            {
                if (current_msg.grenadesRemaining > 0) {
                    // Invia due segnali speciali per notificare al server il lancio di una granata
                    Informations grenade_signal;
                    grenade_signal.ID = -1; // Primo segnale
                    sendInfo(client_socket_fd, &grenade_signal);
                    grenade_signal.ID = -2; // Secondo segnale
                    sendInfo(client_socket_fd, &grenade_signal);
                }
                // Non è un movimento, quindi non invia un aggiornamento di posizione
                has_input = false;
                break;
            }
            default:
                has_input = false;
                break;
        }

        // Se c'è stato un input di movimento valido, invia le nuove coordinate al server
        if (has_input) {
            new_msg.ID = 0; // ID standard per un messaggio di aggiornamento posizione
            if (sendInfo(client_socket_fd, &new_msg) < 0) {
                perror("sendInfo failed");
                break; 
            }
        }
        
        usleep(16000);
    }
}

/*
 * Funzione principale del client.
 * Inizializza ncurses, crea il socket, avvia il loop di input
 * e gestisce la pulizia delle risorse alla fine.
 */
int main() {
    // Attivazione caratteri speciali UNICODE, e inizializzazione ncurses
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    // Crea il socket del client e si connette al server
    int client_socket_fd = createClientSocket();
    if (client_socket_fd == -1) {
        // Se la creazione del socket fallisce, termina
        return 1;
    }

    // Esegue il loop principale di gestione dell'input
    runInputClient(client_socket_fd);

    // Chiude la connessione del socket
    close(client_socket_fd);

    // Termina la modalità ncurses e ripristina lo stato del terminale
    endwin();
    
    // Stampa un messaggio di conferma della terminazione
    printf("Client terminato. Il server ha chiuso la connessione.\n");
    return 0;
}