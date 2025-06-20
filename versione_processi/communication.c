#include "communication.h"

// Wrapper delle funzione read e write per le comunicazione via pipe 

ssize_t readData(int fd, void *data, ssize_t dim) {
    return read(fd, data, dim);
}

ssize_t writeData(int fd, void *data, ssize_t dim) {
    return write(fd, data, dim);
}