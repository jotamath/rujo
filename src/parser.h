// src/parser.h
#ifndef RUJO_PARSER_H
#define RUJO_PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer* l;
    Token cur_token;
    Token peek_token;
} Parser;

void parser_init(Parser* p, Lexer* l);
ASTNode* parser_parse_program(Parser* p);

#endif