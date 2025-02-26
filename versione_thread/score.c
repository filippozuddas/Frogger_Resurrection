#include "librerie.h"
#include "score.h"


void handleScores(Game *game, int countdownTime, int isDead) {
    // Penalità per morte veloce
    if (isDead) {
        if (countdownTime >= 40) {
            game->frog.score -= 50;
        } else if (countdownTime >= 20) {
            game->frog.score -= 25;
        } else {
            game->frog.score -= 10;
        }
        if (game->frog.score < 0) game->frog.score = 0; // Evita punteggi negativi
        return; // Esci dalla funzione perché il punteggio per la vittoria non deve essere applicato
    }

    // Punteggio per la vittoria in base al tempo rimanente
    switch (game->difficulty) {
        case 0: // Facile
            if (countdownTime >= 50) game->frog.score += 100;
            else if (countdownTime >= 40) game->frog.score += 75;
            else if (countdownTime >= 30) game->frog.score += 50;
            else if (countdownTime >= 20) game->frog.score += 25;
            else if (countdownTime >= 10) game->frog.score += 10;
            break;
            case 1: // Normale
            if (countdownTime >= 30) game->frog.score += 150;
            else if (countdownTime >= 35) game->frog.score += 100;
            else if (countdownTime >= 25) game->frog.score += 75;
            else if (countdownTime >= 15) game->frog.score += 50;
            else game->frog.score += 10;
            break;
        case 2: // Difficile
            if (countdownTime >= 10) game->frog.score += 200;
            else if (countdownTime >= 30) game->frog.score += 125;
            else if (countdownTime >= 20) game->frog.score += 100;
            else game->frog.score += 50;
            break;
    }

    loadScores(&scoreList);
    
    // Aggiungi il nuovo punteggio
    addScore(&scoreList, game->frog.score);
    
    // Salva tutti i punteggi
    saveScores(scoreList);


}

ScoreNode* createNode(int score) {
    ScoreNode* newNode = (ScoreNode*)malloc(sizeof(ScoreNode));
    newNode->score = score;
    newNode->next = NULL;
    return newNode;
}

void addScore(ScoreNode** head, int score) {
    ScoreNode* newNode = createNode(score);
    newNode->next = *head;  // Il nuovo nodo punta al vecchio head
    *head = newNode;  // Il nuovo nodo diventa la testa della lista
}

void saveScores(ScoreNode* head) {
    FILE* file = fopen(SCORES_FILE, "wb");
    if (file == NULL) {
        perror("Error opening scores file");
        return;
    }
    
    time_t now;
    time(&now);
    struct tm* timeinfo = localtime(&now);
    
    ScoreEntry entry;
    ScoreNode* current = head;
    int count = 0;
    
    while (current != NULL && count < MAX_SCORES) {
        entry.score = current->score;
        strftime(entry.date, sizeof(entry.date), "Y-%m-%d %H:%M:%S", timeinfo);
        
        fwrite(&entry, sizeof(ScoreEntry), 1, file);
        current = current->next;
        count++;
    }
    
    fclose(file);
}

// Funzione per caricare i punteggi dal file
void loadScores(ScoreNode** head) {
    FILE* file = fopen(SCORES_FILE, "rb");
    if (file == NULL) {
        // File non esiste ancora, non è un errore
        return;
    }
    
    // Pulisci la lista esistente
    while (*head != NULL) {
        ScoreNode* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
    
    ScoreEntry entry;
    while (fread(&entry, sizeof(ScoreEntry), 1, file) == 1) {
        addScore(head, entry.score);
    }
    
    fclose(file);
}

void printDigit(WINDOW *win, int digit, int startX, int startY) {
    const wchar_t **digitSprite = NULL;

    const wchar_t *zero[DIGIT_HEIGHT] = {
        L"█▀▀█",  // Prima riga
        L"█▄▀█",  // Seconda riga
        L"█▄▄█"  // Terza rigA 
    };
    const wchar_t *one[DIGIT_HEIGHT] = {
        L"▄█░",  // Prima riga
        L"░█░",  // Seconda riga
        L"▄█▄"  // Terza riga
    };
    
    const wchar_t *two[DIGIT_HEIGHT] = {
        L"█▀█",  // Prima riga
        L"░▄▀",  // Seconda riga
        L"█▄▄"  // Terza riga
    };
    
    const wchar_t *three[DIGIT_HEIGHT] = {
        L"█▀▀█",  // Prima riga
        L"░░▀▄",  // Seconda riga
        L"█▄▄█"  // Terza riga
    };
    const wchar_t *four[DIGIT_HEIGHT] = {
        L"░█▀█░", // Prima riga
        L"█▄▄█▄",  // Seconda riga
        L"░░░█░"  // Terza riga
    };
    
    const wchar_t *five[DIGIT_HEIGHT] = {
        L"█▀▀",  // Prima riga
        L"▀▀▄",  // Seconda riga
        L"▄▄▀"  // Terza riga
    };
    
    const wchar_t *six[DIGIT_HEIGHT] = {
        L"▄▀▀▄",  // Prima riga
        L"█▄▄░",  // Seconda riga
        L"▀▄▄▀"  // Terza riga
    };
    
    const wchar_t *seven[DIGIT_HEIGHT] = {
        L"▀▀▀█",  // Prima riga
        L"░░█░",  // Seconda riga
        L"░▐▌░"  // Terza riga
    };
    const wchar_t *eight[DIGIT_HEIGHT] = {
        L"▄▀▀▄",  // Prima riga
        L"▄▀▀▄",  // Seconda riga
        L"▀▄▄▀"  // Terza riga
    };
    
    const wchar_t *nine[DIGIT_HEIGHT] = {
        L"▄▀▀▄",  // Prima riga
        L"▀▄▄█",  // Seconda riga
        L"░▄▄▀"  // Terza riga
        };    

    // Scegli la sprite del numero
    switch (digit) {
        case 0:
            digitSprite = zero;
            break;
        case 1:
            digitSprite = one;
            break;
        case 2:
            digitSprite = two;
            break;
        case 3:
            digitSprite = three;
            break;
        case 4:
            digitSprite = four;
            break;
        case 5:
            digitSprite = five;
            break;
        case 6:
            digitSprite = six;
            break;
        case 7:
            digitSprite = seven;
            break;
        case 8:
            digitSprite = eight;
            break;
        case 9:
            digitSprite = nine;
            break;
        default:
            return; // Non fare nulla se il numero non è valido
    }

    // Stampa la sprite scelta
    for (int i = 0; i < DIGIT_HEIGHT; i++) {
        pthread_mutex_lock(&ncurses_mutex);
        mvwprintw(win, startY + i, startX, "%ls", digitSprite[i]);  // Usa "%ls" per wchar_t
        pthread_mutex_unlock(&ncurses_mutex);
    }
}

int digitsCount(int numero) {
    int cifre = 0;
    int temp = numero;
    
    do {
        cifre++;
        temp /= 10;
    } while (temp != 0);
    
    return cifre;
}

void digitsAnalyser(WINDOW *win, int score, int starty, int startx) {
    int temp = score;
    int digits = digitsCount(score);  // Usa il nome corretto
    int digitWidth = 6;  // Aggiungi la larghezza della sprite
    
    int digitArray[10];
    
    // Estrai le cifre
    for (int i = digits - 1; i >= 0; i--) {
        digitArray[i] = temp % 10;
        temp /= 10;
    }
    
    // Stampa dalla prima all'ultima
    for (int i = 0; i < digits; i++) {
        // Correggi l'ordine degli argomenti e aggiungi offset per ogni cifra
        printDigit(win, digitArray[i], startx + (i * digitWidth), starty);
    }
}