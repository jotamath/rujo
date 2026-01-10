#ifndef RUJO_AST_H
#define RUJO_AST_H

#include "lexer.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LIT_INT,
    LIT_FLOAT,
    LIT_STRING,
    LIT_BOOL,
    LIT_CHAR
} LiteralType;

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
    AST_CALL,
    AST_TYPEOF,
    AST_BINARY_OP,
    AST_RETURN,
    AST_IF
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;
    struct ASTNode* next;

    union {
        struct { struct ASTNode* statements; } program;
        struct { char* name; char* type_name; struct ASTNode* value; } var_decl;
        
        struct { 
            LiteralType type;
            int int_val; 
            double float_val;
            char* string_val; 
            bool bool_val;
            uint32_t char_val;
        } literal;

        struct { char* name; struct ASTNode* members; } class_decl;
        struct { char* name; char* return_type; struct ASTNode* params; struct ASTNode* body; } fn_decl;
        struct { struct ASTNode* statements; } block;
        struct { struct ASTNode* target; struct ASTNode* value; } assign;
        struct { struct ASTNode* object; char* member_name; } access;
        struct { char* name; } ident;
        struct { char* name; struct ASTNode* args; } call;
        struct { struct ASTNode* expr; } type_of;
        struct { struct ASTNode* left; char* op; struct ASTNode* right; } binary_op;
        struct { struct ASTNode* value; } ret;
        struct { struct ASTNode* condition; struct ASTNode* then_branch; struct ASTNode* else_branch; } if_stmt;

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
ASTNode* ast_new_literal_float(double value);
ASTNode* ast_new_literal_string(char* value);
ASTNode* ast_new_literal_bool(bool value);
ASTNode* ast_new_literal_char(uint32_t value);

ASTNode* ast_new_assign(ASTNode* target, ASTNode* value);
ASTNode* ast_new_access(ASTNode* object, char* member_name);
ASTNode* ast_new_ident(char* name);
ASTNode* ast_new_call(char* name, ASTNode* args);
ASTNode* ast_new_typeof(ASTNode* expr);

ASTNode* ast_new_binary_op(ASTNode* left, char* op, ASTNode* right);
ASTNode* ast_new_return(ASTNode* value);

ASTNode* ast_new_if(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch);

void ast_print(ASTNode* node, int level);

#endif