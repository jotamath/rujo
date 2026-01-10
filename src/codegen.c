#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

const char* map_type(const char* rujo_type) {
    if (strcmp(rujo_type, "string") == 0) return "const char*";
    if (strcmp(rujo_type, "int")    == 0) return "int";
    if (strcmp(rujo_type, "float")  == 0) return "float";
    if (strcmp(rujo_type, "bool")   == 0) return "bool";
    if (strcmp(rujo_type, "byte")   == 0) return "uint8_t";
    if (strcmp(rujo_type, "char")   == 0) return "uint32_t";
    if (strcmp(rujo_type, "void")   == 0) return "void";
    return rujo_type; 
}

void gen_node(ASTNode* node, FILE* out);

void gen_node(ASTNode* node, FILE* out) {
    if (!node) return;

    switch (node->type) {
        case AST_BLOCK:
            fprintf(out, "{\n");
            ASTNode* stmt = node->data.block.statements;
            while (stmt) {
                gen_node(stmt, out);
                // CORREÇÃO: Adiciona ; se for Chamada OU Atribuição
                if (stmt->type == AST_CALL || stmt->type == AST_ASSIGN) {
                    fprintf(out, ";\n");
                }
                stmt = stmt->next;
            }
            fprintf(out, "}\n");
            break;

        case AST_VAR_DECL:
            fprintf(out, "%s %s", map_type(node->data.var_decl.type_name), node->data.var_decl.name);
            if (node->data.var_decl.value) {
                fprintf(out, " = ");
                gen_node(node->data.var_decl.value, out);
            }
            fprintf(out, ";\n");
            break;

        case AST_ASSIGN:
            gen_node(node->data.assign.target, out);
            fprintf(out, " = ");
            gen_node(node->data.assign.value, out);
            // AST_ASSIGN não gera ; aqui para permitir uso dentro de for(..;..; step)
            break;

        case AST_ACCESS:
            gen_node(node->data.access.object, out);
            if (node->data.access.object->type == AST_IDENTIFIER && 
                strcmp(node->data.access.object->data.ident.name, "this") == 0) {
                fprintf(out, "->%s", node->data.access.member_name);
            } else {
                fprintf(out, ".%s", node->data.access.member_name);
            }
            break;

        case AST_CALL:
            if (strcmp(node->data.call.name, "print") == 0) {
                fprintf(out, "RUJO_PRINT("); 
                if (node->data.call.args) {
                    gen_node(node->data.call.args, out);
                } else {
                    fprintf(out, "\"\"");
                }
                fprintf(out, ")");
            } else {
                fprintf(out, "%s(", node->data.call.name);
                ASTNode* arg = node->data.call.args;
                while (arg) {
                    gen_node(arg, out);
                    if (arg->next) fprintf(out, ", ");
                    arg = arg->next;
                }
                fprintf(out, ")");
            }
            break;

        case AST_IF:
            fprintf(out, "if (");
            gen_node(node->data.if_stmt.condition, out);
            fprintf(out, ") ");
            gen_node(node->data.if_stmt.then_branch, out);
            if (node->data.if_stmt.else_branch) {
                fprintf(out, " else ");
                gen_node(node->data.if_stmt.else_branch, out);
            }
            break;

        case AST_WHILE:
            fprintf(out, "while (");
            gen_node(node->data.while_loop.condition, out);
            fprintf(out, ") ");
            gen_node(node->data.while_loop.body, out);
            break;

        case AST_FOR:
            fprintf(out, "for (");
            if (node->data.for_loop.init) gen_node(node->data.for_loop.init, out);
            else fprintf(out, ";");
            
            fprintf(out, " "); 
            if (node->data.for_loop.condition) gen_node(node->data.for_loop.condition, out);
            fprintf(out, "; ");
            
            if (node->data.for_loop.step) gen_node(node->data.for_loop.step, out);
            fprintf(out, ") ");
            
            gen_node(node->data.for_loop.body, out);
            break;

        case AST_IDENTIFIER:
            fprintf(out, "%s", node->data.ident.name);
            break;

        case AST_LITERAL:
            switch(node->data.literal.type) {
                case LIT_STRING: fprintf(out, "\"%s\"", node->data.literal.string_val); break;
                case LIT_INT:    fprintf(out, "%d", node->data.literal.int_val); break;
                case LIT_FLOAT:  fprintf(out, "%f", node->data.literal.float_val); break;
                case LIT_BOOL:   fprintf(out, "%s", node->data.literal.bool_val ? "true" : "false"); break;
                case LIT_CHAR:   fprintf(out, "%u", node->data.literal.char_val); break;
            }
            break;
        
        case AST_TYPEOF:
            fprintf(out, "RUJO_TYPEOF(");
            gen_node(node->data.type_of.expr, out);
            fprintf(out, ")");
            break;

        case AST_BINARY_OP:
            fprintf(out, "(");
            gen_node(node->data.binary_op.left, out);
            fprintf(out, " %s ", node->data.binary_op.op);
            gen_node(node->data.binary_op.right, out);
            fprintf(out, ")");
            break;

        case AST_RETURN:
            fprintf(out, "return ");
            gen_node(node->data.ret.value, out);
            fprintf(out, ";\n");
            break;

        default:
            break;
    }
}

void gen_structs(ASTNode* node, FILE* out) {
    if (!node) return;
    if (node->type == AST_CLASS_DECL) {
        fprintf(out, "typedef struct {\n");
        ASTNode* member = node->data.class_decl.members;
        while (member) {
            if (member->type == AST_PROP_DECL) {
                fprintf(out, "    %s %s;\n", 
                    map_type(member->data.var_decl.type_name), 
                    member->data.var_decl.name);
            }
            member = member->next;
        }
        fprintf(out, "} %s;\n\n", node->data.class_decl.name);
    }
    gen_structs(node->next, out);
}

void gen_methods(ASTNode* node, FILE* out) {
    if (!node) return;
    
    if (node->type == AST_FN_DECL) {
        if (strcmp(node->data.fn_decl.name, "main") != 0) {
            fprintf(out, "%s %s(", map_type(node->data.fn_decl.return_type), node->data.fn_decl.name);
            ASTNode* param = node->data.fn_decl.params;
            int first = 1;
            while (param) {
                if (!first) fprintf(out, ", ");
                fprintf(out, "%s %s", map_type(param->data.var_decl.type_name), param->data.var_decl.name);
                first = 0;
                param = param->next;
            }
            fprintf(out, ") ");
            gen_node(node->data.fn_decl.body, out);
            fprintf(out, "\n");
        }
    }
    else if (node->type == AST_CLASS_DECL) {
        char* class_name = node->data.class_decl.name;
        ASTNode* member = node->data.class_decl.members;
        while (member) {
            if (member->type == AST_FN_DECL) {
                char func_name[128];
                if (strcmp(member->data.fn_decl.name, "init") == 0) {
                    sprintf(func_name, "%s_init", class_name);
                } else {
                    sprintf(func_name, "%s_%s", class_name, member->data.fn_decl.name);
                }
                fprintf(out, "void %s(%s* this", func_name, class_name);
                ASTNode* param = member->data.fn_decl.params;
                while (param) {
                    fprintf(out, ", %s %s", map_type(param->data.var_decl.type_name), param->data.var_decl.name);
                    param = param->next;
                }
                fprintf(out, ") ");
                gen_node(member->data.fn_decl.body, out);
                fprintf(out, "\n");
            }
            member = member->next;
        }
    }

    gen_methods(node->next, out);
}

void gen_main(ASTNode* node, FILE* out) {
    fprintf(out, "int main() {\n");
    ASTNode* current = node;
    while (current) {
        if (current->type != AST_CLASS_DECL && current->type != AST_FN_DECL) {
            gen_node(current, out);
            // Também adiciona ; no main se for solto
            if (current->type == AST_CALL || current->type == AST_ASSIGN) {
                fprintf(out, ";\n");
            }
        }
        current = current->next;
    }
    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");
}

void codegen_generate(ASTNode* root, FILE* out) {
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <stdint.h>\n");
    fprintf(out, "#include <stdbool.h>\n\n");

    fprintf(out, "void print_int(int x) { printf(\"%%d\\n\", x); }\n");
    fprintf(out, "void print_float(float x) { printf(\"%%f\\n\", x); }\n"); 
    fprintf(out, "void print_string(const char* x) { printf(\"%%s\\n\", x); }\n");
    fprintf(out, "void print_bool(bool x) { printf(\"%%s\\n\", x ? \"true\" : \"false\"); }\n\n");

    fprintf(out, "#define RUJO_PRINT(x) _Generic((x), \\\n");
    fprintf(out, "    int: print_int, \\\n");
    fprintf(out, "    float: print_float, \\\n");
    fprintf(out, "    double: print_float, \\\n");
    fprintf(out, "    bool: print_bool, \\\n");
    fprintf(out, "    char*: print_string, \\\n");
    fprintf(out, "    const char*: print_string \\\n");
    fprintf(out, ")(x)\n\n");

    fprintf(out, "#define RUJO_TYPEOF(x) _Generic((x), \\\n");
    fprintf(out, "    _Bool: \"bool\", \\\n");
    fprintf(out, "    uint8_t: \"byte\", \\\n");
    fprintf(out, "    int: \"int\", \\\n");
    fprintf(out, "    float: \"float\", \\\n");
    fprintf(out, "    double: \"float\", \\\n");
    fprintf(out, "    uint32_t: \"char\", \\\n");
    fprintf(out, "    char*: \"string\", \\\n");
    fprintf(out, "    const char*: \"string\", \\\n");
    fprintf(out, "    default: \"unknown\" \\\n");
    fprintf(out, ")\n\n");

    if (root->type == AST_PROGRAM) {
        gen_structs(root->data.program.statements, out);
        gen_methods(root->data.program.statements, out);
        gen_main(root->data.program.statements, out);
    }
}