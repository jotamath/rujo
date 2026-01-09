#ifndef RUJO_SYMBOL_TABLE_H
#define RUJO_SYMBOL_TABLE_H

typedef enum {
    SYM_VAR,
    SYM_PROP,
    SYM_CLASS,
    SYM_FUNCTION
} SymbolKind;

typedef struct Symbol {
    char* name;
    char* type_name; // "int", "string", "Heroi"
    SymbolKind kind;
    struct Symbol* next; // Lista ligada (colisões ou lista simples)
} Symbol;

typedef struct Scope {
    struct Scope* parent; // Escopo pai (para subir a escada na busca)
    Symbol* symbols;      // Lista de símbolos deste escopo
    int level;            // Debug: nível de indentação
} Scope;

// API
Scope* scope_new(Scope* parent);
void scope_free(Scope* scope);

// Define um símbolo no escopo ATUAL. Retorna 0 se erro (já existe).
int scope_define(Scope* scope, char* name, char* type, SymbolKind kind);

// Busca um símbolo (sobe a escada de escopos se não achar no atual)
Symbol* scope_resolve(Scope* scope, char* name);

#endif