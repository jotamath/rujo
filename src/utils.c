#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* rujo_strndup(const char* s, size_t n) {
    char* d = (char*)malloc(n + 1);
    if (!d) return NULL;
    strncpy(d, s, n);
    d[n] = '\0';
    return d;
}

char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        printf("Erro: Nao foi possivel abrir o arquivo '%s'.\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        printf("Erro: Memoria insuficiente para ler arquivo.\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}