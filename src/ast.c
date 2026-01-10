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
    node->data.literal.type = LIT_INT;
    node->data.literal.int_val = value;
    return node;
}

ASTNode* ast_new_literal_float(double value) {
    ASTNode* node = create_node(AST_LITERAL);
    node->data.literal.type = LIT_FLOAT;
    node->data.literal.float_val = value;
    return node;
}

ASTNode* ast_new_literal_string(char* value) {
    ASTNode* node = create_node(AST_LITERAL);
    node->data.literal.type = LIT_STRING;
    node->data.literal.string_val = strdup(value);
    return node;
}

ASTNode* ast_new_literal_bool(bool value) {
    ASTNode* node = create_node(AST_LITERAL);
    node->data.literal.type = LIT_BOOL;
    node->data.literal.bool_val = value;
    return node;
}

ASTNode* ast_new_literal_char(uint32_t value) {
    ASTNode* node = create_node(AST_LITERAL);
    node->data.literal.type = LIT_CHAR;
    node->data.literal.char_val = value;
    return node;
}

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
    ASTNode* node = create_node(AST_IDENTIFIER);
    node->data.ident.name = strdup(name);
    return node;
}

ASTNode* ast_new_call(char* name, ASTNode* args) {
    ASTNode* node = create_node(AST_CALL);
    node->data.call.name = strdup(name);
    node->data.call.args = args;
    return node;
}

ASTNode* ast_new_typeof(ASTNode* expr) {
    ASTNode* node = create_node(AST_TYPEOF);
    node->data.type_of.expr = expr;
    return node;
}

ASTNode* ast_new_binary_op(ASTNode* left, char* op, ASTNode* right) {
    ASTNode* node = create_node(AST_BINARY_OP);
    node->data.binary_op.left = left;
    node->data.binary_op.op = strdup(op);
    node->data.binary_op.right = right;
    return node;
}

ASTNode* ast_new_return(ASTNode* value) {
    ASTNode* node = create_node(AST_RETURN);
    node->data.ret.value = value;
    return node;
}

ASTNode* ast_new_if(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = create_node(AST_IF);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

// Novos Loops
ASTNode* ast_new_while(ASTNode* condition, ASTNode* body) {
    ASTNode* node = create_node(AST_WHILE);
    node->data.while_loop.condition = condition;
    node->data.while_loop.body = body;
    return node;
}

ASTNode* ast_new_for(ASTNode* init, ASTNode* condition, ASTNode* step, ASTNode* body) {
    ASTNode* node = create_node(AST_FOR);
    node->data.for_loop.init = init;
    node->data.for_loop.condition = condition;
    node->data.for_loop.step = step;
    node->data.for_loop.body = body;
    return node;
}

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
            switch(node->data.literal.type) {
                case LIT_STRING: printf("LiteralString (\"%s\")\n", node->data.literal.string_val); break;
                case LIT_INT:    printf("LiteralInt (%d)\n", node->data.literal.int_val); break;
                case LIT_FLOAT:  printf("LiteralFloat (%.2f)\n", node->data.literal.float_val); break;
                case LIT_BOOL:   printf("LiteralBool (%s)\n", node->data.literal.bool_val ? "true" : "false"); break;
                case LIT_CHAR:   printf("LiteralChar (%u)\n", node->data.literal.char_val); break;
            }
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
        case AST_IDENTIFIER: 
             printf("Ident (%s)\n", node->data.ident.name);
             break;
        case AST_CALL:
            printf("Call (%s)\n", node->data.call.name);
            print_indent(level + 1); 
            printf("Args:\n");
            ast_print(node->data.call.args, level + 2);
            break;
        case AST_TYPEOF:
            printf("TypeOf\n");
            ast_print(node->data.type_of.expr, level + 1);
            break;
        case AST_BINARY_OP:
            printf("BinaryOp (%s)\n", node->data.binary_op.op);
            print_indent(level + 1); printf("Left:\n");
            ast_print(node->data.binary_op.left, level + 2);
            print_indent(level + 1); printf("Right:\n");
            ast_print(node->data.binary_op.right, level + 2);
            break;
        case AST_RETURN:
            printf("Return\n");
            ast_print(node->data.ret.value, level + 1);
            break;
        case AST_IF:
            printf("If\n");
            print_indent(level + 1); printf("Condition:\n");
            ast_print(node->data.if_stmt.condition, level + 2);
            print_indent(level + 1); printf("Then:\n");
            ast_print(node->data.if_stmt.then_branch, level + 2);
            if (node->data.if_stmt.else_branch) {
                print_indent(level + 1); printf("Else:\n");
                ast_print(node->data.if_stmt.else_branch, level + 2);
            }
            break;
        
        case AST_WHILE:
            printf("While\n");
            print_indent(level + 1); printf("Condition:\n");
            ast_print(node->data.while_loop.condition, level + 2);
            print_indent(level + 1); printf("Body:\n");
            ast_print(node->data.while_loop.body, level + 2);
            break;

        case AST_FOR:
            printf("For\n");
            print_indent(level + 1); printf("Init:\n");
            ast_print(node->data.for_loop.init, level + 2);
            print_indent(level + 1); printf("Cond:\n");
            ast_print(node->data.for_loop.condition, level + 2);
            print_indent(level + 1); printf("Step:\n");
            ast_print(node->data.for_loop.step, level + 2);
            print_indent(level + 1); printf("Body:\n");
            ast_print(node->data.for_loop.body, level + 2);
            break;

        default:
            printf("Unknown Node\n");
    }

    if (node->next) {
        ast_print(node->next, level);
    }
}