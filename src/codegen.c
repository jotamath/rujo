// src/codegen.c
#include "codegen.h"
#include <stdlib.h>
#include <string.h>

// Mapeia tipos do Rujo para C
const char* map_type(const char* rujo_type) {
    if (strcmp(rujo_type, "string") == 0) return "char*";
    if (strcmp(rujo_type, "int") == 0) return "int";
    if (strcmp(rujo_type, "void") == 0) return "void";
    // Classes são passadas como ponteiros (struct Name*)
    return rujo_type; 
}

// Forward decl
void gen_node(ASTNode* node, FILE* out);

// Gera código para expressões e statements
void gen_node(ASTNode* node, FILE* out) {
    if (!node) return;

    switch (node->type) {
        case AST_BLOCK:
            fprintf(out, "{\n");
            ASTNode* stmt = node->data.block.statements;
            while (stmt) {
                gen_node(stmt, out);
                // CORREÇÃO: Chamadas dentro de blocos precisam de ponto e vírgula
                if (stmt->type == AST_CALL) fprintf(out, ";\n");
                stmt = stmt->next;
            }
            fprintf(out, "}\n");
            break;

        case AST_VAR_DECL:
            // Rujo: string s = "a"; -> C: char* s = "a";
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
            fprintf(out, ";\n");
            break;

        case AST_ACCESS:
            gen_node(node->data.access.object, out);
            
            // Hack para v0.1: Se o objeto for 'this', usa seta. Senão, ponto.
            if (node->data.access.object->type == AST_IDENTIFIER && 
                strcmp(node->data.access.object->data.ident.name, "this") == 0) {
                fprintf(out, "->%s", node->data.access.member_name);
            } else {
                fprintf(out, ".%s", node->data.access.member_name);
            }
            break;

        // --- NOVO: GERAÇÃO DE CHAMADA DE FUNÇÃO ---
        case AST_CALL:
            // Tradução nativa: print -> printf
            if (strcmp(node->data.call.name, "print") == 0) {
                fprintf(out, "printf(\"%%s\\n\", "); // printf("%s\n", arg)
                
                if (node->data.call.args) {
                    gen_node(node->data.call.args, out);
                } else {
                    fprintf(out, "\"\"");
                }
                fprintf(out, ")");
            } else {
                // Chamada normal
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
        // ------------------------------------------

        case AST_IDENTIFIER:
            fprintf(out, "%s", node->data.ident.name);
            break;

        case AST_LITERAL:
            if (node->data.literal.string_val) {
                fprintf(out, "\"%s\"", node->data.literal.string_val);
            } else {
                fprintf(out, "%d", node->data.literal.int_val);
            }
            break;

        default:
            break;
    }
}

// Gera definições de structs (Passo 1)
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

// Gera funções/métodos (Passo 2)
void gen_methods(ASTNode* node, FILE* out) {
    if (!node) return;
    if (node->type == AST_CLASS_DECL) {
        char* class_name = node->data.class_decl.name;
        ASTNode* member = node->data.class_decl.members;
        while (member) {
            if (member->type == AST_FN_DECL) {
                // Rujo: init -> C: NomeClasse_init
                char func_name[128];
                if (strcmp(member->data.fn_decl.name, "init") == 0) {
                    sprintf(func_name, "%s_init", class_name);
                } else {
                    sprintf(func_name, "%s_%s", class_name, member->data.fn_decl.name);
                }

                fprintf(out, "void %s(%s* this", func_name, class_name);
                
                // Params
                ASTNode* param = member->data.fn_decl.params;
                while (param) {
                    fprintf(out, ", %s %s", 
                        map_type(param->data.var_decl.type_name), 
                        param->data.var_decl.name);
                    param = param->next;
                }
                fprintf(out, ") ");
                
                // Body
                gen_node(member->data.fn_decl.body, out);
                fprintf(out, "\n");
            }
            member = member->next;
        }
    }
    gen_methods(node->next, out);
}

// Gera o main e código solto (Passo 3)
void gen_main(ASTNode* node, FILE* out) {
    fprintf(out, "int main() {\n");
    
    ASTNode* current = node;
    while (current) {
        // Gera apenas o que NÃO for classe
        if (current->type != AST_CLASS_DECL) {
            gen_node(current, out);
            
            // CORREÇÃO: Adiciona ; se for uma chamada solta no top-level (ex: print)
            if (current->type == AST_CALL) {
                fprintf(out, ";\n");
            }
        }
        current = current->next;
    }

    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");
}

void codegen_generate(ASTNode* root, FILE* out) {
    // Cabeçalho C padrão
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n\n");

    // 1. Definição das Structs
    if (root->type == AST_PROGRAM) {
        gen_structs(root->data.program.statements, out);
        // 2. Definição dos Métodos
        gen_methods(root->data.program.statements, out);
        // 3. Função Main
        gen_main(root->data.program.statements, out);
    }
}