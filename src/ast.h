// src/ast.h
#ifndef RUJO_AST_H
#define RUJO_AST_H

#include "lexer.h"

typedef enum {
    AST_PROGRAM,
    AST_VAR_DECL,
    AST_LITERAL,
    AST_CLASS_DECL,
    AST_PROP_DECL,
    AST_FN_DECL,
    AST_BLOCK,
    AST_ASSIGN,
    AST_ACCESS,
    AST_IDENTIFIER,
    AST_CALL        // <--- CERTIFIQUE-SE QUE ISSO ESTÁ AQUI
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;
    struct ASTNode* next;

    union {
        struct { struct ASTNode* statements; } program;
        struct { char* name; char* type_name; struct ASTNode* value; } var_decl;
        struct { int int_val; char* string_val; } literal;
        struct { char* name; struct ASTNode* members; } class_decl;
        struct { char* name; char* return_type; struct ASTNode* params; struct ASTNode* body; } fn_decl;
        struct { struct ASTNode* statements; } block;
        struct { struct ASTNode* target; struct ASTNode* value; } assign;
        struct { struct ASTNode* object; char* member_name; } access;
        struct { char* name; } ident;
        
        // --- NOVO ---
        struct { 
            char* name; 
            struct ASTNode* args; 
        } call;
        // ------------
    } data;
};

// Construtores
ASTNode* ast_new_program(ASTNode* statements);
ASTNode* ast_new_var_decl(char* name, char* type, ASTNode* value);
ASTNode* ast_new_prop_decl(char* name, char* type);
ASTNode* ast_new_class_decl(char* name, ASTNode* members);
ASTNode* ast_new_fn_decl(char* name, char* ret_type, ASTNode* params, ASTNode* body);
ASTNode* ast_new_block(ASTNode* statements);
ASTNode* ast_new_literal_int(int value);
ASTNode* ast_new_literal_string(char* value);
ASTNode* ast_new_assign(ASTNode* target, ASTNode* value);
ASTNode* ast_new_access(ASTNode* object, char* member_name);
ASTNode* ast_new_ident(char* name);

// --- NOVO PROTÓTIPO ---
ASTNode* ast_new_call(char* name, ASTNode* args);
// ----------------------

void ast_print(ASTNode* node, int level);

#endif