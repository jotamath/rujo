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
        free(current);
        current = next;
    }
    free(scope);
}

int scope_define(Scope* scope, char* name, char* type, SymbolKind kind) {
    Symbol* s = scope->symbols;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            return 0; 
        }
        s = s->next;
    }

    Symbol* new_sym = (Symbol*)malloc(sizeof(Symbol));
    new_sym->name = name; 
    new_sym->type_name = type;
    new_sym->kind = kind;
    
    new_sym->next = scope->symbols;
    scope->symbols = new_sym;
    return 1; 
}

Symbol* scope_resolve(Scope* scope, char* name) {
    Symbol* s = scope->symbols;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            return s;
        }
        s = s->next;
    }

    if (scope->parent) {
        return scope_resolve(scope->parent, name);
    }

    return NULL;
}