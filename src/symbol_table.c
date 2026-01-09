// src/symbol_table.c
#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Scope* scope_new(Scope* parent) {
    Scope* s = (Scope*)malloc(sizeof(Scope));
    s->parent = parent;
    s->symbols = NULL;
    s->level = parent ? parent->level + 1 : 0;
    return s;
}

void scope_free(Scope* scope) {
    Symbol* current = scope->symbols;
    while (current) {
        Symbol* next = current->next;
        // free(current->name); // Nomes vêm da AST, cuidado com double free
        // free(current->type_name);
        free(current);
        current = next;
    }
    free(scope);
}

int scope_define(Scope* scope, char* name, char* type, SymbolKind kind) {
    // 1. Verifica se já existe no escopo ATUAL (redeclare error)
    Symbol* s = scope->symbols;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            return 0; // Erro: Símbolo já existe neste escopo
        }
        s = s->next;
    }

    // 2. Cria novo símbolo
    Symbol* new_sym = (Symbol*)malloc(sizeof(Symbol));
    new_sym->name = name; // Aponta para a string da AST (não duplica)
    new_sym->type_name = type;
    new_sym->kind = kind;
    
    // 3. Adiciona na cabeça da lista
    new_sym->next = scope->symbols;
    scope->symbols = new_sym;
    return 1; // Sucesso
}

Symbol* scope_resolve(Scope* scope, char* name) {
    // Busca no escopo atual
    Symbol* s = scope->symbols;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            return s;
        }
        s = s->next;
    }

    // Se não achou e tem pai, busca no pai (Recursão/Escada)
    if (scope->parent) {
        return scope_resolve(scope->parent, name);
    }

    return NULL; // Não encontrado
}