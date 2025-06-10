
#include "librerie.h"

int main() {
    //attiva i caratteri speciali dell'UNICODE
    setlocale(LC_ALL, ""); 
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    setColors();

    /* Inizializzazione dell'audio per le musiche di gioco */
    if (!initAudio()) {
        endwin();
        exit(1);
    }
    
    int server_socket_fd = createServerSocket();
    if (server_socket_fd == -1) {
        endwin();
        printf("Errore: impossibile creare il socket del server.\n");
        exit(1);
    }
    printf("Server avviato. In attesa di connessione dal client sulla porta 8088\n");

    int game_socket_fd = acceptClientConnection(server_socket_fd);
    if (game_socket_fd == -1) {
        endwin();
        printf("Errore: impossibile accettare la connessione del client.\n");
        close(server_socket_fd);
        exit(1);
    }
    clear();
    refresh();
    
    Game game; 
    int choice; 

    startMusic("../music/WELCOME.wav");
    animate_welcome();
    stopMusic();
    
    startMusic("../music/MENU1.wav");

    do {

        choice = mainMenu(&game);

        if (choice == 1) {
            stopMusic();
            initGame(&game);
            runGame(&game, game_socket_fd);
            stopGame(&game);
            startMusic("../music/MENU1.wav");
        }
        else if (choice == 2) {
            levelMenu(&game);
        }
        else if (choice == 3) {
            displayScoreMenu(&game);
        }
    } while (choice != 4);

    // Invia segnale di terminazione al client quando l'utente esce
    Informations exit_signal;
    exit_signal.ID = -99; // Codice speciale per indicare terminazione
    sendInfo(game_socket_fd, &exit_signal);
    
    close(game_socket_fd);
    close(server_socket_fd);

    endwin();
    terminateAudio();
    return 0;
}