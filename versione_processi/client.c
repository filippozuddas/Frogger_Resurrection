#include "socket.h"
#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <locale.h>

static void runInputClient(int client_socket_fd) {
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    Informations current_pos;
    current_pos.x = 0;
    current_pos.y = 0;
    current_pos.grenadesRemaining = 0;
    
    Informations new_pos;

    while (true) {
        receiveInfo(client_socket_fd, &current_pos);

        int input = getch();

        bool has_input = true;
        new_pos = current_pos;

        switch (input) {
            case 'w': 
            case 'W': 
            case KEY_UP:
                new_pos.y = (current_pos.y > 0) ? current_pos.y - FROG_HEIGHT : current_pos.y;
                break;
            case 's': 
            case 'S': 
            case KEY_DOWN:
                new_pos.y = (current_pos.y < GAME_HEIGHT - FROG_HEIGHT - 1) ? current_pos.y + FROG_HEIGHT : current_pos.y;
                break;
            
            case 'a': 
            case 'A': 
            case KEY_LEFT:
                new_pos.x = (current_pos.x > 0) ? current_pos.x - 1 : current_pos.x;
                break;
            
            case 'd': 
            case 'D': 
            case KEY_RIGHT:
                new_pos.x = (current_pos.x < GAME_WIDTH - FROG_WIDTH) ? current_pos.x + 1 : current_pos.x;
                break;
            case ' ':
            {
                if (current_pos.grenadesRemaining > 0) {
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
            new_pos.ID = 0;
            if (sendInfo(client_socket_fd, &new_pos) < 0) {
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
    printf("Client terminato.\n");
    return 0;
}