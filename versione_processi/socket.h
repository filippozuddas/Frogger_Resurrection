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

int createServerSocket();
int acceptClientConnection(int server_fd);
int createClientSocket();
ssize_t sendInfo(int socket_fd, const Informations* msg);
ssize_t receiveInfo(int socket_fd, Informations* msg);