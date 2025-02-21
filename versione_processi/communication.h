#pragma once

#include <unistd.h>

ssize_t readData(int fd, void *data, ssize_t dim);
ssize_t writeData(int fd, void *data, ssize_t dim);