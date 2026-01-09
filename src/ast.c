#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ASTNode* create_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->next = NULL;
    return node;
}

// ... (Mantenha ast_new_program, var_decl, literals iguais ao anterior) ...
// Vou repetir apenas os novos para economizar espaço, mantenha os antigos!

ASTNode* ast_new_program(ASTNode* statements) {
    ASTNode* node = create_node(AST_PROGRAM);
    node->data.program.statements = statements;
    return node;
}

ASTNode* ast_new_var_decl(char* name, char* type, ASTNode* value) {
    ASTNode* node = create_node(AST_VAR_DECL);
    node->data.var_decl.name = strdup(name);
    node->data.var_decl.type_name = strdup(type);
    node->data.var_decl.value = value;
    return node;
}

ASTNode* ast_new_literal_int(int value) {
    ASTNode* node = create_node(AST_LITERAL);
    node->data.literal.int_val = value;
    node->data.literal.string_val = NULL;
    return node;
}

ASTNode* ast_new_literal_string(char* value) {
    ASTNode* node = create_node(AST_LITERAL);
    node->data.literal.string_val = strdup(value);
    return node;
}

// --- NOVOS CONSTRUTORES ---

ASTNode* ast_new_prop_decl(char* name, char* type) {
    ASTNode* node = create_node(AST_PROP_DECL);
    node->data.var_decl.name = strdup(name);
    node->data.var_decl.type_name = strdup(type);
    node->data.var_decl.value = NULL;
    return node;
}

ASTNode* ast_new_class_decl(char* name, ASTNode* members) {
    ASTNode* node = create_node(AST_CLASS_DECL);
    node->data.class_decl.name = strdup(name);
    node->data.class_decl.members = members;
    return node;
}

ASTNode* ast_new_fn_decl(char* name, char* ret_type, ASTNode* params, ASTNode* body) {
    ASTNode* node = create_node(AST_FN_DECL);
    node->data.fn_decl.name = strdup(name);
    node->data.fn_decl.return_type = strdup(ret_type);
    node->data.fn_decl.params = params;
    node->data.fn_decl.body = body;
    return node;
}

ASTNode* ast_new_block(ASTNode* statements) {
    ASTNode* node = create_node(AST_BLOCK);
    node->data.block.statements = statements;
    return node;
}

ASTNode* ast_new_assign(ASTNode* target, ASTNode* value) {
    ASTNode* node = create_node(AST_ASSIGN);
    node->data.assign.target = target;
    node->data.assign.value = value;
    return node;
}

ASTNode* ast_new_access(ASTNode* object, char* member_name) {
    ASTNode* node = create_node(AST_ACCESS);
    node->data.access.object = object;
    node->data.access.member_name = strdup(member_name);
    return node;
}

ASTNode* ast_new_ident(char* name) {
    ASTNode* node = create_node(AST_IDENTIFIER); // AST_IDENTIFIER precisa estar no enum do .h se não estiver
    node->data.ident.name = strdup(name);
    return node;
}

ASTNode* ast_new_call(char* name, ASTNode* args) {
    ASTNode* node = create_node(AST_CALL);
    node->data.call.name = strdup(name);
    node->data.call.args = args;
    return node;
}

// --- VISUALIZAÇÃO ATUALIZADA ---

void print_indent(int level) {
    for (int i = 0; i < level; i++) printf("  ");
}

void ast_print(ASTNode* node, int level) {
    if (!node) return;

    print_indent(level);

    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            ast_print(node->data.program.statements, level + 1);
            break;
        case AST_VAR_DECL:
            printf("VarDecl (%s %s)\n", node->data.var_decl.type_name, node->data.var_decl.name);
            if (node->data.var_decl.value) ast_print(node->data.var_decl.value, level + 1);
            break;
        case AST_PROP_DECL:
            printf("Prop (%s %s)\n", node->data.var_decl.type_name, node->data.var_decl.name);
            break;
        case AST_LITERAL:
            if (node->data.literal.string_val) printf("LiteralString (\"%s\")\n", node->data.literal.string_val);
            else printf("LiteralInt (%d)\n", node->data.literal.int_val);
            break;       
        case AST_CLASS_DECL:
            printf("Class (%s)\n", node->data.class_decl.name);
            ast_print(node->data.class_decl.members, level + 1);
            break;
        case AST_FN_DECL:
            printf("Function/Init (%s) -> %s\n", node->data.fn_decl.name, node->data.fn_decl.return_type);
            printf("  Params:\n");
            ast_print(node->data.fn_decl.params, level + 2);
            printf("  Body:\n");
            ast_print(node->data.fn_decl.body, level + 2);
            break;
        case AST_BLOCK:
            printf("Block\n");
            ast_print(node->data.block.statements, level + 1);
            break;

        case AST_ASSIGN:
            printf("Assign (=)\n");
            print_indent(level + 1); printf("Target:\n");
            ast_print(node->data.assign.target, level + 2);
            print_indent(level + 1); printf("Value:\n");
            ast_print(node->data.assign.value, level + 2);
            break;

        case AST_ACCESS:
            printf("Access (field: %s)\n", node->data.access.member_name);
            print_indent(level + 1); printf("Object:\n");
            ast_print(node->data.access.object, level + 2);
            break;

        case AST_IDENTIFIER: // Adicione AST_IDENTIFIER no enum do .h se esqueceu
             printf("Ident (%s)\n", node->data.ident.name);
             break;
        
        case AST_CALL:
            printf("Call (%s)\n", node->data.call.name);
            print_indent(level + 1); 
            printf("Args:\n");
            ast_print(node->data.call.args, level + 2);
            break;
            
        default:
            printf("Unknown Node\n");
    }

    if (node->next) {
        ast_print(node->next, level);
    }
}