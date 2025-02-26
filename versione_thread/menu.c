#include "librerie.h"
#include "menu.h"


static int startx; // Dichiarate come statiche per ridurre la visibilità
static int starty;

int width = 130;  // Riduci un po' la larghezza della finestra
int height = 35;  // Riduci un po' l'altezza della finestra



void showMenu(Game *game) {
    setlocale(LC_ALL, ""); // Only once, here
    initscr();
    cbreak();
    noecho();
    curs_set(0); // Only once, here
    keypad(stdscr, TRUE); // Correct place, and now protected

    animate_welcome();
    mainMenu(game);
    endwin();

}

void animate_welcome() {
    curs_set(0);
    nodelay(stdscr, TRUE);
    setlocale(LC_ALL, "");
    
const wchar_t *sprite[MAX_HEIGHT_WELCOME] = {

    L"  ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░▒▓█▓▒░      ░▒▓██████▓▒░ ░▒▓██████▓▒░░▒▓██████████████▓▒░░▒▓████████▓▒░      ░▒▓████████▓▒░▒▓██████▓▒░             ",
    L"  ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░     ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░                ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░            ",
    L"  ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░     ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░                ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░            ",
    L"  ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓██████▓▒░ ░▒▓█▓▒░     ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓██████▓▒░           ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░            ",
    L"  ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░     ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░                ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░            ",
    L"  ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░     ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░                ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░            ",
    L"   ░▒▓█████████████▓▒░░▒▓████████▓▒░▒▓████████▓▒░▒▓██████▓▒░ ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░         ░▒▓█▓▒░   ░▒▓██████▓▒░             ",
    L"                                                                                                                                                    ",
    L"                                                                                                                                                    ",
    L"                        ░▒▓████████▓▒░▒▓███████▓▒░ ░▒▓██████▓▒░ ░▒▓██████▓▒░ ░▒▓██████▓▒░░▒▓████████▓▒░▒▓███████▓▒░                                  ",
    L"                        ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░                                 ",
    L"                        ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░                                 ",
    L"                        ░▒▓██████▓▒░ ░▒▓███████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒▒▓███▓▒░▒▓█▓▒▒▓███▓▒░▒▓██████▓▒░ ░▒▓███████▓▒░                                  ",
    L"                        ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░                                 ",
    L"                        ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░                                 ",
    L"                        ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓██████▓▒░ ░▒▓██████▓▒░ ░▒▓██████▓▒░░▒▓████████▓▒░▒▓█▓▒░░▒▓█▓▒░                                 ",
    L"                                                                                                                                                    ",
    L"                                                                                                                                                    ",
    L" ░▒▓███████▓▒░░▒▓████████▓▒░░▒▓███████▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓███████▓▒░░▒▓███████▓▒░░▒▓████████▓▒░▒▓██████▓▒░▒▓████████▓▒░▒▓█▓▒░░▒▓██████▓▒░░▒▓███████▓▒░",
    L" ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░     ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░   ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░",
    L" ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░     ░▒▓█▓▒░        ░▒▓█▓▒░   ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░",
    L" ░▒▓███████▓▒░░▒▓██████▓▒░  ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓███████▓▒░░▒▓███████▓▒░░▒▓██████▓▒░░▒▓█▓▒░        ░▒▓█▓▒░   ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░",
    L" ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░             ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░     ░▒▓█▓▒░        ░▒▓█▓▒░   ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░",
    L" ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░             ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░     ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░   ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░",
    L" ░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░▒▓███████▓▒░ ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░▒▓██████▓▒░  ░▒▓█▓▒░   ░▒▓█▓▒░░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░",                                                                                                                                                               
    };
    
    starty = 15;
    startx = 115;

    for (int i = 0; i < MAX_HEIGHT_WELCOME; i++) {
        mvaddwstr(starty + i, startx, sprite[i]); // Ora dovrebbe funzionare
        refresh();
        usleep(300000);

        if (getch() != ERR) {
            break;
        }
    }

    clear();
    refresh();

    curs_set(0);
}

void print_menu(WINDOW *menu_win, int highlight, MenuOption menu[], int n_choices) {
    int x = 1, y = 1;
    if (menu == restart) {
        x = 45;
        y = 30;
    }

    for (int i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE);
            for (int j = 0; j < 5; ++j) {
                mvwprintw(menu_win, y + j, x, "%ls", menu[i].text[j]);
            }
            wattroff(menu_win, A_REVERSE);
        } else {
            for (int j = 0; j < 5; ++j) {
                mvwprintw(menu_win, y + j, x, "%ls", menu[i].text[j]);
            }
        }
        y += 7;
    }
    wrefresh(menu_win);
}


void mainMenu(Game *game) {
    init_window_position();
    WINDOW *menu_win = newwin(GAME_HEIGHT, GAME_WIDTH, starty, startx); // Usa le costanti
    int highlight = 1;
    int choice = 0;
    int c;

    nodelay(menu_win, TRUE); // Usa menu_win
    keypad(menu_win, TRUE);
    mvprintw(LINES - 2, 0, "Usa le frecce per muoverti; Invio per selezionare");
    refresh();

    int n_choices = sizeof(menu) / sizeof(MenuOption);
    print_menu(menu_win, highlight, menu, n_choices);

    while ((c = wgetch(menu_win)) != 0) { // Leggi dalla finestra corretta
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    --highlight;
                break;

            case KEY_DOWN:
                if (highlight == n_choices)
                    highlight = 1;
                else
                    ++highlight;
                break;

            case 10:  // Invio
            choice = highlight;
            if (choice == 1) {
                wclear(menu_win);
                wrefresh(menu_win);
    
                // Debug: stampa un messaggio per verificare se startGame() viene chiamata
                printf("Chiamo startGame()\n");
    
                startGame(game);
    
                // Debug: stampa un messaggio per verificare se startGame() è terminata
                printf("startGame() terminata\n");
    
                // Torna al menu
                print_menu(menu_win, highlight, menu, n_choices);
                wrefresh(menu_win);
                }
                else if (choice == 2) {
                    pthread_mutex_lock(&ncurses_mutex);
                    wclear(menu_win);
                    wrefresh(menu_win);
                    pthread_mutex_unlock(&ncurses_mutex);
                    levelMenu(game);
                    return;
                }
                else if (choice == 3) {
                    pthread_mutex_lock(&ncurses_mutex);
                    wclear(menu_win);
                    wrefresh(menu_win);
                    pthread_mutex_unlock(&ncurses_mutex);
                    displayScoreMenu(game);
                    return;
                }
                else if (choice == 4) {
                    return;  // Esci dal menu
                }
                break;

            default:
                break;
        }

        // Rileggi l'input dopo l'elaborazione dell'azione
        print_menu(menu_win, highlight, menu, n_choices);
        wrefresh(menu_win); // Assicurati che il menu venga aggiornato dopo ogni cambiamento
        }
    }

void levelMenu(Game *game){

    init_window_position();
    int highlight = 1;
    int choice = 0;
    int c;
    WINDOW *level_win = newwin(GAME_HEIGHT, GAME_WIDTH, starty, startx);
    keypad(level_win, TRUE);
    mvprintw(LINES - 2, 0, "Usa le frecce per muoverti; Invio per selezionare");
    refresh();
    
    
    int n_choices = sizeof(level) / sizeof(MenuOption);
    
    print_menu(level_win, highlight, level , n_choices);
    while((c = wgetch(level_win)) != 0){
        switch(c) {
            case KEY_UP:
                if(highlight == 1)
                    highlight = n_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if(highlight == n_choices)
                    highlight = 1;
                else 
                    ++highlight;
                break;
            case 10: // Tasto Invio
                choice = highlight;
                if(choice == 1){
                    pthread_mutex_lock(&ncurses_mutex);
                    wclear(level_win);
                    wrefresh(level_win);
                    pthread_mutex_unlock(&ncurses_mutex);
                    game->difficulty = 0;
                    mainMenu(game);
                    return;
                }
                else if(choice == 2){
                    pthread_mutex_lock(&ncurses_mutex);
                    wclear(level_win);
                    wrefresh(level_win);
                    pthread_mutex_unlock(&ncurses_mutex);
                    game->difficulty = 1;
                    mainMenu(game);
                    return;
                }
                else if(choice == 3){
                    pthread_mutex_lock(&ncurses_mutex);
                    wclear(level_win);
                    wrefresh(level_win);
                    pthread_mutex_unlock(&ncurses_mutex);
                    game->difficulty = 2;
                    mainMenu(game);
                    return;
                }
                else if(choice == 0){
                    pthread_mutex_lock(&ncurses_mutex);
                    wclear(level_win);
                    wrefresh(level_win);
                    pthread_mutex_unlock(&ncurses_mutex);
                    mainMenu(game);
                    return;
                }
                break;
            default:
                break;
        }
        print_menu(level_win, highlight, level, n_choices);
        if(choice != 0) // Esci dal ciclo se è stata fatta una scelta
            break;
    }
}

void init_window_position() {
    startx = (COLS - width) / 2;
    starty = (LINES - height) / 2;
}

void displayScoreMenu(Game *game) {

    init_window_position();
    pthread_mutex_lock(&ncurses_mutex);
    WINDOW *score_win = newwin(height + 10, width, (LINES - height) / 2, (COLS - width) / 2);
    keypad(score_win, TRUE);
    
    // Carica i punteggi dal file
    FILE* file = fopen(SCORES_FILE, "rb");
    
    box(score_win, 0, 0);
    wattron(score_win, A_BOLD);
    mvwprintw(score_win, 2, (height + 70) / 2, "🏆 HIGH SCORES TABLE 🏆");
    
    if (file == NULL) {
        mvwprintw(score_win, height/2, (width - 20) / 2, "No scores yet!");
    } else {
        ScoreEntry entry;
        int y = 5;
        int count = 1;
        
        // Intestazioni della tabella
        mvwprintw(score_win, y, 5,  " RANK  ");
        mvwprintw(score_win, y, 20, " SCORE ");
        mvwprintw(score_win, y, 40, "    DATE    ");
        mvwprintw(score_win, y, 65, " DIFFICULTY ");
        y += 2;
        
        // Linea separatrice
        for (int x = 5; x < GAME_WIDTH - 45; x++) {
            mvwprintw(score_win, y - 1, x, "="); // Usa '=' invece di '-'
        }
        
        while (fread(&entry, sizeof(ScoreEntry), 1, file) == 1 && count <= MAX_SCORES) {
            const char* difficulty;
            switch(entry.difficulty) {
                case 0: difficulty = "Easy"; break;
                case 1: difficulty = "Medium"; break;
                case 2: difficulty = "Hard"; break;
                default: difficulty = "Unknown"; break;
            }
            
            mvwprintw(score_win, y, 10, "%d.", count);
            mvwprintw(score_win, y, 25, "%d", entry.score);
            mvwprintw(score_win, y, 45, "%s", entry.date);
            mvwprintw(score_win, y, 70, "%s", difficulty);
            y++;
            count++;
        }
        fclose(file);
    }
    
    // Istruzioni in fondo alla finestra
    mvwprintw(score_win, height-3, (width - 35) / 2, "Press any key to return to main menu");
    wattroff(score_win, A_BOLD);
    
    wrefresh(score_win);
    
    // Aspetta input dall'utente
    wgetch(score_win);
    
    // Pulisci e ritorna al menu principale
    wclear(score_win);
    wrefresh(score_win);
    delwin(score_win);
    pthread_mutex_unlock(&ncurses_mutex);
    mainMenu(game);
}

void startGame(Game *game) {

    init_synchro();
    // Debug: stampa un messaggio all'inizio di initGame()
    printf("Inizio initGame()\n");

    initGame(game);

    // Debug: stampa un messaggio alla fine di initGame()
    printf("Fine initGame()\n");

    // Debug: stampa un messaggio all'inizio di runGame()
    printf("Inizio runGame()\n");

    runGame(game);

    // Debug: stampa un messaggio alla fine di runGame()
    printf("Fine runGame()\n");

    printf("Chiamato stopGame()\n");
    stopGame(game);
    deallocate_synchro();

}