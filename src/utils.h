#ifndef RUJO_UTILS_H
#define RUJO_UTILS_H

#include <stddef.h>

char* read_file(const char* filename);
char* rujo_strndup(const char* s, size_t n);

#endif