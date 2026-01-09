// src/semantic.c
#include "semantic.h"
#include "symbol_table.h"
#include <stdio.h>
#include <string.h>

// Variável global temporária para contagem de erros
static int error_count = 0;

void sem_error(char* msg, char* detail) {
    printf("[Erro Semantico] %s: %s\n", msg, detail);
    error_count++;
}

// Forward declaration
void check_node(ASTNode* node, Scope* scope);

void check_block(ASTNode* block, Scope* parent_scope) {
    Scope* local_scope = scope_new(parent_scope);
    ASTNode* stmt = block->data.block.statements;
    while (stmt) {
        check_node(stmt, local_scope);
        stmt = stmt->next;
    }
    // scope_free(local_scope);
}

void check_node(ASTNode* node, Scope* scope) {
    if (!node) return;

    switch (node->type) {
        case AST_PROGRAM: {
            // Cria escopo global
            Scope* global = scope_new(NULL);
            
            // --- NOVO: Registra funções nativas (Built-ins) ---
            scope_define(global, "print", "void", SYM_FUNCTION);
            // --------------------------------------------------

            ASTNode* stmt = node->data.program.statements;
            while (stmt) {
                check_node(stmt, global);
                stmt = stmt->next;
            }
            break;
        }

        case AST_VAR_DECL:
            if (!scope_define(scope, node->data.var_decl.name, node->data.var_decl.type_name, SYM_VAR)) {
                sem_error("Variavel redeclarada no mesmo escopo", node->data.var_decl.name);
            }
            if (node->data.var_decl.value) {
                check_node(node->data.var_decl.value, scope);
            }
            break;

        case AST_CLASS_DECL:
            if (!scope_define(scope, node->data.class_decl.name, "class", SYM_CLASS)) {
                sem_error("Classe ja definida", node->data.class_decl.name);
            }
            Scope* class_scope = scope_new(scope);
            ASTNode* member = node->data.class_decl.members;
            
            scope_define(class_scope, "this", node->data.class_decl.name, SYM_VAR);

            while (member) {
                if (member->type == AST_PROP_DECL) {
                    if (!scope_define(class_scope, member->data.var_decl.name, member->data.var_decl.type_name, SYM_PROP)) {
                        sem_error("Propriedade duplicada", member->data.var_decl.name);
                    }
                }
                else if (member->type == AST_FN_DECL) {
                    check_node(member, class_scope);
                }
                member = member->next;
            }
            break;

        case AST_FN_DECL: {
            Scope* fn_scope = scope_new(scope);
            ASTNode* param = node->data.fn_decl.params;
            while (param) {
                scope_define(fn_scope, param->data.var_decl.name, param->data.var_decl.type_name, SYM_VAR);
                param = param->next;
            }
            if (node->data.fn_decl.body) {
                check_node(node->data.fn_decl.body, fn_scope); 
            }
            break;
        }

        case AST_BLOCK:
            check_block(node, scope);
            break;

        case AST_ASSIGN:
            check_node(node->data.assign.value, scope);
            check_node(node->data.assign.target, scope);
            break;

        // --- NOVO CASE PARA CHAMADA DE FUNÇÃO ---
        case AST_CALL:
            // 1. Verifica se a função existe (ex: print)
            if (!scope_resolve(scope, node->data.call.name)) {
                sem_error("Funcao nao declarada", node->data.call.name);
            }
            // 2. Verifica os argumentos (recursivamente)
            ASTNode* arg = node->data.call.args;
            while (arg) {
                check_node(arg, scope);
                arg = arg->next;
            }
            break;
        // ----------------------------------------

        case AST_IDENTIFIER:
            if (strcmp(node->data.ident.name, "this") == 0) {
                if (!scope_resolve(scope, "this")) {
                     sem_error("Uso de 'this' fora de classe", "this");
                }
            } else {
                if (!scope_resolve(scope, node->data.ident.name)) {
                    sem_error("Variavel nao declarada", node->data.ident.name);
                }
            }
            break;
        
        case AST_ACCESS:
            check_node(node->data.access.object, scope);
            break;

        default:
            break;
    }
}

int semantic_analysis(ASTNode* root) {
    error_count = 0;
    check_node(root, NULL);
    return error_count == 0;
}