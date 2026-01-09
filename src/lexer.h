#ifndef RUJO_LEXER_H
#define RUJO_LEXER_H

#include <stddef.h>
#include <stdint.h>

// --- Definição dos Tokens (Baseado no LDD v1.4) ---
typedef enum {
    TOK_EOF,
    TOK_ILLEGAL,

    // Identificadores e Literais
    TOK_IDENT,          // nomeVariavel, NomeClasse
    TOK_LIT_INT,        // 10
    TOK_LIT_FLOAT,      // 2.5
    TOK_LIT_STRING,     // "Texto"
    TOK_LIT_CHAR,       // 'c'

    // Keywords de Estrutura
    TOK_FN,             // fn
    TOK_CLASS,          // class
    TOK_PROP,           // prop
    TOK_INIT,           // init
    TOK_MODULE,         // module
    TOK_IMPORT,         // import
    TOK_PUB,            // pub
    TOK_RETURN,         // return
    TOK_REQUIRED,       // required (para DTOs)
    TOK_ANNOTATION,     // annotation (keyword para definir anotações)

    // Keywords de Controle de Fluxo
    TOK_IF,             // if
    TOK_ELSE,           // else
    // (Adicionaremos while/for futuramente conforme necessidade)

    // Keywords de Tipos Primitivos (LDD 3.2)
    TOK_TYPE_INT,       // int
    TOK_TYPE_FLOAT,     // float
    TOK_TYPE_BOOL,      // bool
    TOK_TYPE_BYTE,      // byte
    TOK_TYPE_CHAR,      // char
    TOK_TYPE_STRING,    // string (agora primitivo/keyword)
    TOK_TYPE_VOID,      // void

    // Literais Booleanos e Nulo
    TOK_TRUE,           // true
    TOK_FALSE,          // false
    TOK_NULL,           // null

    // Operadores e Delimitadores
    TOK_ASSIGN,         // =
    TOK_PLUS,           // +
    TOK_MINUS,          // -
    TOK_STAR,           // *
    TOK_SLASH,          // /
    TOK_BANG,           // !
    TOK_DOT,            // .
    TOK_COMMA,          // ,
    TOK_COLON,          // :
    TOK_SEMICOLON,      // ; (Obrigatório na v1.4)
    TOK_QUESTION,       // ? (Null safety)
    TOK_AT,             // @ (Uso de annotation)
    TOK_AMPERSAND,      // & (Borrow)

    // Blocos
    TOK_LPAREN,         // (
    TOK_RPAREN,         // )
    TOK_LBRACE,         // {
    TOK_RBRACE,         // }
    TOK_LBRACKET,       // [
    TOK_RBRACKET,       // ]
    
    // Comparação
    TOK_EQ,             // ==
    TOK_NEQ,            // !=
    TOK_LT,             // <
    TOK_GT,             // >
    TOK_LTE,            // <=
    TOK_GTE             // >=

} TokenType;

// Estrutura do Token
typedef struct {
    TokenType type;
    const char* literal; // Aponta para o texto original no source
    int length;          // Tamanho do literal
    int line;
    int column;
} Token;

// Estado do Lexer
typedef struct {
    const char* input;
    size_t input_len;
    size_t position;      // Posição atual (char atual)
    size_t read_position; // Próxima posição (lookahead)
    char ch;              // Char atual sendo analisado
    int line;
    int column;
} Lexer;

// Funções Públicas
void lexer_init(Lexer* l, const char* input);
Token lexer_next_token(Lexer* l);
const char* token_type_to_str(TokenType type);

#endif