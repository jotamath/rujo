// src/codegen.h
#ifndef RUJO_CODEGEN_H
#define RUJO_CODEGEN_H

#include "ast.h"
#include <stdio.h>

// Recebe a raiz da AST e um arquivo aberto para escrita
void codegen_generate(ASTNode* root, FILE* out);

#endif