#include "socket.h"

/**
 * Crea un socket server in ascolto sulla porta definita (PORT).
 * Configura il socket per accettare connessioni.
 */
int createServerSocket() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // Crea un nuovo socket TCP/IP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }
    // Imposta le opzioni per riutilizzare indirizzo e porta
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }

    // Configura l'indirizzo del server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accetta connessioni su qualsiasi interfaccia
    address.sin_port = htons(PORT);

    // Associa il socket all'indirizzo configurato
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }
    // Mette il socket in ascolto (massimo 1 connessione in coda)
    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

/**
 * Accetta una connessione da un client.
 */
int acceptClientConnection(int server_fd) {
    int client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Accetta la connessione in arrivo
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        return -1;
    }
    printf("Client connesso.\n");
    return client_fd;
}

/**
 * Crea un socket client e si connette al server locale (127.0.0.1)
 */
int createClientSocket() {
    int client_fd;
    struct sockaddr_in serv_addr;

    // Crea un nuovo socket TCP/IP
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Client socket creation error");
        return -1;
    }
    // Configura l'indirizzo del server a cui connettersi
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(client_fd);
        return -1;
    }
    // Effettua la connessione al server
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        close(client_fd);
        return -1;
    }
    return client_fd;
}

/**
 * Invia informazioni attraverso un socket.
 */
ssize_t sendInfo(int socket_fd, const Informations* msg) {
    return send(socket_fd, msg, sizeof(Informations), 0);
}

/**
 * Riceve informazioni da un socket in modo non bloccante.
 */
ssize_t receiveInfo(int socket_fd, Informations* msg) {
    // MSG_DONTWAIT rende la chiamata non bloccante
    return recv(socket_fd, msg, sizeof(Informations), MSG_DONTWAIT);
}
