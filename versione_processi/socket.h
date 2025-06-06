#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "struct.h"

#define PORT 8088 // Porta che useremo per la comunicazione

/**
 * @brief Crea un socket server, esegue il bind e lo mette in ascolto.
 * @return Il file descriptor del socket server, o -1 in caso di errore.
 */
int createServerSocket();

/**
 * @brief Accetta una connessione da un client. Funzione bloccante.
 * @param server_fd Il file descriptor del socket server in ascolto.
 * @return Il file descriptor del socket del client connesso, o -1 in caso di errore.
 */
int acceptClientConnection(int server_fd);

/**
 * @brief Crea un socket client e si connette al server.
 * @return Il file descriptor del socket client, o -1 in caso di errore.
 */
int createClientSocket();

/**
 * @brief Invia una struttura Informations attraverso il socket specificato.
 * @param socket_fd Il file descriptor del socket.
 * @param msg Il puntatore alla struttura da inviare.
 * @return L'esito della funzione send().
 */
ssize_t sendInfo(int socket_fd, const Informations* msg);

/**
 * @brief Riceve una struttura Informations dal socket specificato in modalità non bloccante.
 * @param socket_fd Il file descriptor del socket.
 * @param msg Il puntatore alla struttura dove salvare i dati ricevuti.
 * @return L'esito della funzione recv().
 */
ssize_t receiveInfo(int socket_fd, Informations* msg);