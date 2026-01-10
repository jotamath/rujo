#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Forward declarations
ASTNode* parse_statement(Lexer* l);
ASTNode* parse_expression(Lexer* l);
char* parse_type_name(Lexer* l);

ASTNode* parse_expression(Lexer* l);
ASTNode* parse_equality(Lexer* l);
ASTNode* parse_comparison(Lexer* l);
ASTNode* parse_term(Lexer* l);
ASTNode* parse_factor(Lexer* l);
ASTNode* parse_primary(Lexer* l);

Token curr_tok;

void parser_init(Lexer* l) {
    (void)l;
}

void next_token(Lexer* l) {
    curr_tok = lexer_next_token(l);
}

void expect(Lexer* l, TokenType type) {
    if (curr_tok.type == type) {
        next_token(l);
    } else {
        printf("Erro de Sintaxe: Esperado %s, encontrado %s na linha %d\n", 
            token_type_to_str(type), token_type_to_str(curr_tok.type), curr_tok.line);
        exit(1);
    }
}

char* parse_type_name(Lexer* l) {
    char* type_name = NULL;
    switch (curr_tok.type) {
        case TOK_TYPE_INT:    type_name = "int"; break;
        case TOK_TYPE_FLOAT:  type_name = "float"; break;
        case TOK_TYPE_BOOL:   type_name = "bool"; break;
        case TOK_TYPE_BYTE:   type_name = "byte"; break;
        case TOK_TYPE_CHAR:   type_name = "char"; break;
        case TOK_TYPE_STRING: type_name = "string"; break;
        case TOK_TYPE_VOID:   type_name = "void"; break;
        case TOK_IDENT:       
            type_name = rujo_strndup(curr_tok.literal, curr_tok.length); 
            break;
        default:
            printf("Erro: Esperado um tipo válido na linha %d\n", curr_tok.line);
            exit(1);
    }
    next_token(l);
    return type_name;
}

// --- EXPRESSÕES (Precedência) ---

ASTNode* parse_primary(Lexer* l) {
    ASTNode* node = NULL;

    switch (curr_tok.type) {
        case TOK_LIT_INT:
            {
                int val = strtol(curr_tok.literal, NULL, 10);
                node = ast_new_literal_int(val);
                next_token(l);
            }
            break;
        
        case TOK_LIT_FLOAT:
            {
                double val = strtod(curr_tok.literal, NULL);
                node = ast_new_literal_float(val);
                next_token(l);
            }
            break;

        case TOK_LIT_STRING:
            {
                char* s = rujo_strndup(curr_tok.literal + 1, curr_tok.length - 2);
                node = ast_new_literal_string(s);
                next_token(l);
            }
            break;

        case TOK_LIT_BOOL:
            {
                bool val = (strncmp(curr_tok.literal, "true", 4) == 0);
                node = ast_new_literal_bool(val);
                next_token(l);
            }
            break;

        case TOK_LIT_CHAR:
            {
                char c = curr_tok.literal[1]; 
                node = ast_new_literal_char((uint32_t)c);
                next_token(l);
            }
            break;

        case TOK_IDENT:
            {
                char* name = rujo_strndup(curr_tok.literal, curr_tok.length);
                next_token(l);
                
                if (curr_tok.type == TOK_LPAREN) {
                    next_token(l); // consome '('
                    ASTNode* args = NULL;
                    ASTNode* last_arg = NULL;
                    
                    if (curr_tok.type != TOK_RPAREN) {
                        while (1) {
                            ASTNode* arg = parse_expression(l);
                            if (!args) args = arg;
                            else last_arg->next = arg;
                            last_arg = arg;

                            if (curr_tok.type == TOK_COMMA) {
                                next_token(l);
                            } else {
                                break;
                            }
                        }
                    }
                    expect(l, TOK_RPAREN);
                    node = ast_new_call(name, args);
                } else {
                    node = ast_new_ident(name);
                }
            }
            break;
        
        case TOK_TYPEOF:
            {
                next_token(l);
                expect(l, TOK_LPAREN);
                ASTNode* expr = parse_expression(l);
                expect(l, TOK_RPAREN);
                node = ast_new_typeof(expr);
            }
            break;

        case TOK_LPAREN:
            {
                next_token(l);
                node = parse_expression(l);
                expect(l, TOK_RPAREN);
            }
            break;

        default:
            printf("Erro: Token inesperado em expressão na linha %d: %s\n", curr_tok.line, token_type_to_str(curr_tok.type));
            exit(1);
    }
    return node;
}

ASTNode* parse_factor(Lexer* l) {
    ASTNode* left = parse_primary(l);

    while (curr_tok.type == TOK_STAR || curr_tok.type == TOK_SLASH) {
        char* op = (curr_tok.type == TOK_STAR) ? "*" : "/";
        next_token(l);
        ASTNode* right = parse_primary(l);
        left = ast_new_binary_op(left, op, right);
    }
    return left;
}

ASTNode* parse_term(Lexer* l) {
    ASTNode* left = parse_factor(l);

    while (curr_tok.type == TOK_PLUS || curr_tok.type == TOK_MINUS) {
        char* op = (curr_tok.type == TOK_PLUS) ? "+" : "-";
        next_token(l);
        ASTNode* right = parse_factor(l);
        left = ast_new_binary_op(left, op, right);
    }
    return left;
}

ASTNode* parse_comparison(Lexer* l) {
    ASTNode* left = parse_term(l);

    while (curr_tok.type == TOK_LT || curr_tok.type == TOK_GT ||
           curr_tok.type == TOK_LTE || curr_tok.type == TOK_GTE) {
        
        char* op = NULL;
        if (curr_tok.type == TOK_LT) op = "<";
        else if (curr_tok.type == TOK_GT) op = ">";
        else if (curr_tok.type == TOK_LTE) op = "<=";
        else if (curr_tok.type == TOK_GTE) op = ">=";

        next_token(l);
        ASTNode* right = parse_term(l);
        left = ast_new_binary_op(left, op, right);
    }
    return left;
}

ASTNode* parse_equality(Lexer* l) {
    ASTNode* left = parse_comparison(l);

    while (curr_tok.type == TOK_EQ || curr_tok.type == TOK_NEQ) {
        char* op = (curr_tok.type == TOK_EQ) ? "==" : "!=";
        next_token(l);
        ASTNode* right = parse_comparison(l);
        left = ast_new_binary_op(left, op, right);
    }
    return left;
}

ASTNode* parse_expression(Lexer* l) {
    return parse_equality(l);
}

// --- STATEMENTS (Declarações) ---

ASTNode* parse_var_decl(Lexer* l) {
    char* type = parse_type_name(l); 
    
    if (curr_tok.type != TOK_IDENT) {
        printf("Erro: Esperado nome da variável após tipo.\n");
        exit(1);
    }
    char* name = rujo_strndup(curr_tok.literal, curr_tok.length);
    next_token(l);

    ASTNode* value = NULL;
    if (curr_tok.type == TOK_ASSIGN) {
        next_token(l);
        value = parse_expression(l);
    }

    expect(l, TOK_SEMICOLON);
    return ast_new_var_decl(name, type, value);
}

ASTNode* parse_statement(Lexer* l) {
    // Declaração de Variáveis
    if (curr_tok.type == TOK_TYPE_INT || 
        curr_tok.type == TOK_TYPE_FLOAT ||
        curr_tok.type == TOK_TYPE_BOOL || 
        curr_tok.type == TOK_TYPE_BYTE ||
        curr_tok.type == TOK_TYPE_CHAR ||
        curr_tok.type == TOK_TYPE_STRING) {
        return parse_var_decl(l);
    }

    // Identificadores (Chamadas ou Atribuições)
    if (curr_tok.type == TOK_IDENT) {
        ASTNode* expr = parse_expression(l);
        
        if (curr_tok.type == TOK_ASSIGN && expr->type == AST_IDENTIFIER) {
            next_token(l); // consome =
            ASTNode* value = parse_expression(l);
            expect(l, TOK_SEMICOLON);
            return ast_new_assign(expr, value);
        }

        expect(l, TOK_SEMICOLON);
        return expr;
    }

    // Blocos
    if (curr_tok.type == TOK_LBRACE) {
        next_token(l);
        ASTNode* stmts = NULL;
        ASTNode* last = NULL;
        while (curr_tok.type != TOK_RBRACE && curr_tok.type != TOK_EOF) {
            ASTNode* stmt = parse_statement(l);
            if (!stmts) stmts = stmt;
            else last->next = stmt;
            last = stmt;
        }
        expect(l, TOK_RBRACE);
        return ast_new_block(stmts);
    }

    // IF
    if (curr_tok.type == TOK_IF) {
        next_token(l);
        expect(l, TOK_LPAREN);
        ASTNode* condition = parse_expression(l);
        expect(l, TOK_RPAREN);
        
        ASTNode* then_branch = parse_statement(l);
        ASTNode* else_branch = NULL;

        if (curr_tok.type == TOK_ELSE) {
            next_token(l);
            else_branch = parse_statement(l);
        }

        return ast_new_if(condition, then_branch, else_branch);
    }

    // WHILE
    if (curr_tok.type == TOK_WHILE) {
        next_token(l); // consome while
        expect(l, TOK_LPAREN);
        ASTNode* condition = parse_expression(l);
        expect(l, TOK_RPAREN);
        ASTNode* body = parse_statement(l);
        return ast_new_while(condition, body);
    }

    // FOR (C-Style)
    if (curr_tok.type == TOK_FOR) {
        next_token(l);
        expect(l, TOK_LPAREN);
        
        ASTNode* init = NULL;
        if (curr_tok.type == TOK_TYPE_INT) { 
             init = parse_var_decl(l);
        } else {
             if (curr_tok.type != TOK_SEMICOLON) {
                 init = parse_expression(l);
             }
             if (curr_tok.type == TOK_SEMICOLON) next_token(l); 
        }

        ASTNode* cond = NULL;
        if (curr_tok.type != TOK_SEMICOLON) {
            cond = parse_expression(l);
        }
        expect(l, TOK_SEMICOLON);

        ASTNode* step = NULL;
        if (curr_tok.type != TOK_RPAREN) {
            ASTNode* expr = parse_expression(l);
            if (curr_tok.type == TOK_ASSIGN && expr->type == AST_IDENTIFIER) {
                next_token(l);
                ASTNode* val = parse_expression(l);
                step = ast_new_assign(expr, val);
            } else {
                step = expr;
            }
        }
        expect(l, TOK_RPAREN);

        ASTNode* body = parse_statement(l);
        return ast_new_for(init, cond, step, body);
    }

    // Funções
    if (curr_tok.type == TOK_FN) {
        next_token(l);
        char* name = rujo_strndup(curr_tok.literal, curr_tok.length);
        next_token(l);
        
        expect(l, TOK_LPAREN);
        
        ASTNode* params = NULL;
        ASTNode* last_param = NULL;

        if (curr_tok.type != TOK_RPAREN) {
            while (1) {
                char* p_type = parse_type_name(l);
                char* p_name = rujo_strndup(curr_tok.literal, curr_tok.length);
                expect(l, TOK_IDENT);

                ASTNode* p_node = ast_new_var_decl(p_name, p_type, NULL);
                if (!params) params = p_node;
                else last_param->next = p_node;
                last_param = p_node;

                if (curr_tok.type == TOK_COMMA) {
                    next_token(l);
                } else {
                    break;
                }
            }
        }

        expect(l, TOK_RPAREN);
        expect(l, TOK_COLON);
        char* ret_type = parse_type_name(l);
        
        ASTNode* body = parse_statement(l);
        return ast_new_fn_decl(name, ret_type, params, body);
    }

    // Return
    if (curr_tok.type == TOK_RETURN) {
        next_token(l);
        ASTNode* val = parse_expression(l);
        expect(l, TOK_SEMICOLON);
        return ast_new_return(val);
    }

    printf("Erro: Statement desconhecido na linha %d ('%s')\n", curr_tok.line, token_type_to_str(curr_tok.type));
    exit(1);
    return NULL;
}

ASTNode* parser_parse_program(Lexer* l) {
    next_token(l); 
    ASTNode* head = NULL;
    ASTNode* current = NULL;

    while (curr_tok.type != TOK_EOF) {
        ASTNode* stmt = parse_statement(l);
        if (stmt) {
            if (!head) head = stmt;
            else current->next = stmt;
            current = stmt;
        }
    }
    return ast_new_program(head);
}