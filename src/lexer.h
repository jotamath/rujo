#ifndef RUJO_LEXER_H
#define RUJO_LEXER_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    TOK_EOF,
    TOK_ILLEGAL,

    TOK_IDENT,
    TOK_LIT_INT,
    TOK_LIT_FLOAT,
    TOK_LIT_STRING,
    TOK_LIT_CHAR,
    TOK_LIT_BOOL,

    TOK_FN,
    TOK_CLASS,
    TOK_PROP,
    TOK_INIT,
    TOK_MODULE,
    TOK_IMPORT,
    TOK_PUB,
    TOK_RETURN,
    TOK_REQUIRED,
    TOK_ANNOTATION,

    TOK_TYPEOF,

    TOK_IF,
    TOK_ELSE,

    TOK_TYPE_INT,
    TOK_TYPE_FLOAT,
    TOK_TYPE_BOOL,
    TOK_TYPE_BYTE,
    TOK_TYPE_CHAR,
    TOK_TYPE_STRING,
    TOK_TYPE_VOID,
    TOK_NULL,

    TOK_ASSIGN,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_BANG,
    TOK_DOT,
    TOK_COMMA,
    TOK_COLON,
    TOK_SEMICOLON,
    TOK_QUESTION,
    TOK_AT,
    TOK_AMPERSAND,

    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    
    TOK_EQ,
    TOK_NEQ,
    TOK_LT,
    TOK_GT,
    TOK_LTE,
    TOK_GTE

} TokenType;

typedef struct {
    TokenType type;
    const char* literal;
    int length;
    int line;
    int column;
} Token;

typedef struct {
    const char* input;
    size_t input_len;
    size_t position;
    size_t read_position;
    char ch;
    int line;
    int column;
} Lexer;

void lexer_init(Lexer* l, const char* input);
Token lexer_next_token(Lexer* l);
const char* token_type_to_str(TokenType type);

#endif