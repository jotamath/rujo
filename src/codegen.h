#ifndef RUJO_CODEGEN_H
#define RUJO_CODEGEN_H

#include "ast.h"
#include <stdio.h>

void codegen_generate(ASTNode* root, FILE* out);

#endif