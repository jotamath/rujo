#ifndef RUJO_PARSER_H
#define RUJO_PARSER_H

#include "lexer.h"
#include "ast.h"

void parser_init(Lexer* l);
ASTNode* parser_parse_program(Lexer* l);

#endif