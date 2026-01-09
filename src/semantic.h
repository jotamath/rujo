// src/semantic.h
#ifndef RUJO_SEMANTIC_H
#define RUJO_SEMANTIC_H

#include "ast.h"

// Retorna 1 se sucesso, 0 se encontrou erros semânticos
int semantic_analysis(ASTNode* root);

#endif