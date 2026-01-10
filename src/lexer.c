#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h> 

// --- Helpers Internos ---

void lexer_init(Lexer* l, const char* input) {
    l->input = input;
    l->input_len = strlen(input);
    l->position = 0;
    l->read_position = 0;
    l->line = 1;
    l->column = 0;
    
    // Prime o primeiro caractere
    if (l->read_position >= l->input_len) {
        l->ch = 0;
    } else {
        l->ch = l->input[l->read_position];
    }
    l->read_position++;
}

void read_char(Lexer* l) {
    if (l->read_position >= l->input_len) {
        l->ch = 0; // EOF
    } else {
        l->ch = l->input[l->read_position];
    }
    l->position = l->read_position;
    l->read_position++;
    l->column++;
}

char peek_char(Lexer* l) {
    if (l->read_position >= l->input_len) {
        return 0;
    }
    return l->input[l->read_position];
}

void skip_whitespace(Lexer* l) {
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
        if (l->ch == '\n') {
            l->line++;
            l->column = 0;
        }
        read_char(l);
    }
}

// Verifica Keywords do LDD v1.4
TokenType lookup_ident(const char* ident, int length) {
    // Helper macro para comparar strings de tamanho fixo
    #define CHECK_KEYWORD(str, type) \
        if (length == (int)strlen(str) && strncmp(ident, str, length) == 0) return type;

    // Tipos Primitivos
    CHECK_KEYWORD("int", TOK_TYPE_INT);
    CHECK_KEYWORD("float", TOK_TYPE_FLOAT);
    CHECK_KEYWORD("bool", TOK_TYPE_BOOL);
    CHECK_KEYWORD("byte", TOK_TYPE_BYTE);
    CHECK_KEYWORD("char", TOK_TYPE_CHAR);
    CHECK_KEYWORD("string", TOK_TYPE_STRING); 
    CHECK_KEYWORD("void", TOK_TYPE_VOID);

    // Estrutura
    CHECK_KEYWORD("fn", TOK_FN);
    CHECK_KEYWORD("class", TOK_CLASS);
    CHECK_KEYWORD("prop", TOK_PROP);
    CHECK_KEYWORD("init", TOK_INIT);
    CHECK_KEYWORD("return", TOK_RETURN);
    CHECK_KEYWORD("pub", TOK_PUB);
    CHECK_KEYWORD("import", TOK_IMPORT);
    CHECK_KEYWORD("module", TOK_MODULE);
    CHECK_KEYWORD("required", TOK_REQUIRED);
    CHECK_KEYWORD("annotation", TOK_ANNOTATION);

    // Controle e Valores
    CHECK_KEYWORD("if", TOK_IF);
    CHECK_KEYWORD("else", TOK_ELSE);
    CHECK_KEYWORD("true", TOK_LIT_BOOL);
    CHECK_KEYWORD("false", TOK_LIT_BOOL);
    CHECK_KEYWORD("null", TOK_NULL);
    
    CHECK_KEYWORD("typeOf", TOK_TYPEOF);

    return TOK_IDENT;
}

Token lexer_next_token(Lexer* l) {
    Token tok;
    
    // Loop para pular whitespace E comentários recursivamente
    while (1) {
        skip_whitespace(l);

        // Verifica se é comentário de linha //
        if (l->ch == '/' && peek_char(l) == '/') {
            // Consome até o fim da linha
            while (l->ch != '\n' && l->ch != 0) {
                read_char(l);
            }
            // O loop volta para o topo e consome o \n no skip_whitespace
            continue; 
        }
        break; // Se não for whitespace nem comentário, segue para ler o token
    }

    // Configuração inicial do token
    tok.line = l->line;
    tok.column = l->column;
    tok.literal = &l->input[l->position]; 
    tok.length = 1;

    switch (l->ch) {
        case '=':
            if (peek_char(l) == '=') {
                read_char(l); tok.length = 2; tok.type = TOK_EQ;
            } else { tok.type = TOK_ASSIGN; }
            break;
        case '+': tok.type = TOK_PLUS; break;
        case '-': tok.type = TOK_MINUS; break;
        case '*': tok.type = TOK_STAR; break;
        
        // CORREÇÃO: Slash simples agora é divisão
        case '/': tok.type = TOK_SLASH; break; 
        
        case '!':
            if (peek_char(l) == '=') {
                read_char(l); tok.length = 2; tok.type = TOK_NEQ;
            } else { tok.type = TOK_BANG; }
            break;
        case '<':
            if (peek_char(l) == '=') {
                read_char(l); tok.length = 2; tok.type = TOK_LTE;
            } else { tok.type = TOK_LT; }
            break;
        case '>':
             if (peek_char(l) == '=') {
                read_char(l); tok.length = 2; tok.type = TOK_GTE;
            } else { tok.type = TOK_GT; }
            break;
        case ';': tok.type = TOK_SEMICOLON; break;
        case ':': tok.type = TOK_COLON; break;
        case ',': tok.type = TOK_COMMA; break;
        case '.': tok.type = TOK_DOT; break;
        case '?': tok.type = TOK_QUESTION; break;
        case '@': tok.type = TOK_AT; break;
        case '&': tok.type = TOK_AMPERSAND; break;
        case '(': tok.type = TOK_LPAREN; break;
        case ')': tok.type = TOK_RPAREN; break;
        case '{': tok.type = TOK_LBRACE; break;
        case '}': tok.type = TOK_RBRACE; break;
        case '[': tok.type = TOK_LBRACKET; break;
        case ']': tok.type = TOK_RBRACKET; break;
        
        case '\'':
            tok.type = TOK_LIT_CHAR;
            tok.literal = &l->input[l->position];
            read_char(l); 
            if (l->ch != '\'' && l->ch != 0) { read_char(l); }
            if (l->ch == '\'') { read_char(l); }
            tok.length = (int)(&l->input[l->position] - tok.literal);
            return tok;

        case '"':
            // Leitura de String Literal
            tok.type = TOK_LIT_STRING;
            tok.literal = &l->input[l->position]; 
            read_char(l); 
            int len = 1;
            while (l->ch != '"' && l->ch != 0) { read_char(l); len++; }
            if (l->ch == '"') { read_char(l); len++; }
            tok.length = len;
            return tok; 
            
        case 0:
            tok.type = TOK_EOF;
            tok.literal = "";
            tok.length = 0;
            break;
            
        default:
            if (isalpha(l->ch) || l->ch == '_') {
                int start_pos = l->position;
                int len = 0;
                while (isalnum(l->ch) || l->ch == '_') {
                    read_char(l);
                    len++;
                }
                tok.type = lookup_ident(&l->input[start_pos], len);
                tok.literal = &l->input[start_pos];
                tok.length = len;
                return tok; 
            } else if (isdigit(l->ch)) {
                int start_pos = l->position;
                int len = 0;
                TokenType type = TOK_LIT_INT;
                while (isdigit(l->ch)) {
                    read_char(l);
                    len++;
                }
                if (l->ch == '.') {
                    type = TOK_LIT_FLOAT;
                    read_char(l);
                    len++;
                    while (isdigit(l->ch)) {
                        read_char(l);
                        len++;
                    }
                }
                tok.type = type;
                tok.literal = &l->input[start_pos];
                tok.length = len;
                return tok;
            } else {
                tok.type = TOK_ILLEGAL;
            }
    }

    read_char(l);
    return tok;
}

const char* token_type_to_str(TokenType type) {
    switch(type) {
        case TOK_EOF: return "EOF";
        case TOK_ILLEGAL: return "ILLEGAL";
        case TOK_IDENT: return "IDENT";
        case TOK_LIT_INT: return "LIT_INT";
        case TOK_LIT_FLOAT: return "LIT_FLOAT";
        case TOK_LIT_STRING: return "LIT_STRING";
        case TOK_LIT_CHAR: return "LIT_CHAR";
        case TOK_LIT_BOOL: return "LIT_BOOL";
        case TOK_FN: return "FN";
        case TOK_CLASS: return "CLASS";
        case TOK_PROP: return "PROP";
        case TOK_INIT: return "INIT";
        case TOK_RETURN: return "RETURN";
        case TOK_TYPE_INT: return "TYPE_INT";
        case TOK_TYPE_FLOAT: return "TYPE_FLOAT";
        case TOK_TYPE_STRING: return "TYPE_STRING";
        case TOK_TYPE_VOID: return "TYPE_VOID";
        case TOK_ASSIGN: return "ASSIGN";
        case TOK_PLUS: return "PLUS";
        case TOK_MINUS: return "MINUS";
        case TOK_STAR: return "STAR";
        case TOK_SLASH: return "SLASH";
        case TOK_SEMICOLON: return "SEMICOLON";
        case TOK_COLON: return "COLON";
        case TOK_COMMA: return "COMMA";
        case TOK_DOT: return "DOT";
        case TOK_LPAREN: return "LPAREN";
        case TOK_RPAREN: return "RPAREN";
        case TOK_LBRACE: return "LBRACE";
        case TOK_RBRACE: return "RBRACE";
        case TOK_IF: return "IF";
        case TOK_ELSE: return "ELSE";
        case TOK_TYPEOF: return "TYPEOF";

        case TOK_EQ: return "EQ (==)";
        case TOK_NEQ: return "NEQ (!=)";
        case TOK_LT: return "LT (<)";
        case TOK_GT: return "GT (>)";
        case TOK_LTE: return "LTE (<=)";
        case TOK_GTE: return "GTE (>=)";

        default: return "TOKEN";
    }
}