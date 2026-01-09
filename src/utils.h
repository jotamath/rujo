// src/utils.h
#ifndef RUJO_UTILS_H
#define RUJO_UTILS_H

#include <stddef.h> // Para size_t

// Lê todo o conteúdo de um arquivo para uma string
char* read_file(const char* path);

// Duplica uma string com tamanho limitado (seguro)
char* rujo_strndup(const char* s, size_t n);

#endif