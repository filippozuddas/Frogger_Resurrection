#include "socket.h"
#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <locale.h>

static void runInputClient(int client_socket_fd) {
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    Informations current_msg;
    current_msg.x = 0;
    current_msg.y = 0;
    current_msg.grenadesRemaining = 0;
    
    Informations new_msg;

    while (true) {
        ssize_t recv_result = receiveInfo(client_socket_fd, &current_msg);
        
        // Verifica se il server ha inviato un segnale di terminazione
        if (recv_result > 0 && current_msg.ID == -99) {
            // Ricevuto segnale di terminazione dal server
            break;
        }

        int input = getch();

        bool has_input = true;
        new_msg = current_msg;

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
            case ' ':
            {
                if (current_msg.grenadesRemaining > 0) {
                    Informations grenade_signal;
                    grenade_signal.ID = -1;
                    sendInfo(client_socket_fd, &grenade_signal);
                    grenade_signal.ID = -2;
                    sendInfo(client_socket_fd, &grenade_signal);
                }
                has_input = false;
                break;
            }
            default:
                has_input = false;
                break;
        }

        if (has_input) {
            new_msg.ID = 0;
            if (sendInfo(client_socket_fd, &new_msg) < 0) {
                perror("sendInfo failed");
                break; 
            }
        }
        
        usleep(16000);
    }
}

int main() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    int client_socket_fd = createClientSocket();
    if (client_socket_fd == -1) {
        return 1;
    }

    runInputClient(client_socket_fd);

    close(client_socket_fd);
    endwin();
    printf("Client terminato. Il server ha chiuso la connessione.\n");
    return 0;
}