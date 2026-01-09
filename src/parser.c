// src/parser.c
#include "parser.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void next_token(Parser* p) {
    p->cur_token = p->peek_token;
    p->peek_token = lexer_next_token(p->l);
}

void parser_init(Parser* p, Lexer* l) {
    p->l = l;
    lexer_init(l, l->input); 
    p->cur_token = lexer_next_token(l);
    p->peek_token = lexer_next_token(l);
}

int cur_token_is(Parser* p, TokenType t) { return p->cur_token.type == t; }
int peek_token_is(Parser* p, TokenType t) { return p->peek_token.type == t; }

int expect_peek(Parser* p, TokenType t) {
    if (peek_token_is(p, t)) {
        next_token(p);
        return 1;
    } else {
        printf("[Erro Parser L%d] Esperado %s, encontrado %s\n", 
            p->peek_token.line, token_type_to_str(t), token_type_to_str(p->peek_token.type));
        return 0;
    }
}

// --- FORWARD DECLARATIONS (CRUCIAIS PARA EVITAR ERROS) ---
ASTNode* parse_statement(Parser* p);
ASTNode* parse_block(Parser* p);
ASTNode* parse_expression(Parser* p);
ASTNode* parse_call_args(Parser* p); // <--- A CORREÇÃO ESTÁ AQUI
// ---------------------------------------------------------

// --- PARSING FUNCTIONS ---

// src/parser.c

ASTNode* parse_call_args(Parser* p) {
    // Caso especial: chamada vazia "()"
    if (peek_token_is(p, TOK_RPAREN)) {
        next_token(p); // consome ')'
        return NULL;
    }
    
    next_token(p); // Salta o '(' para o primeiro argumento

    ASTNode* head = NULL;
    ASTNode* tail = NULL;

    while (1) {
        ASTNode* arg = parse_expression(p);
        
        // parse_expression já avançou o cursor. 
        // Agora 'cur_token' deve ser ',' ou ')'
        
        if (arg) {
            if (!head) { head = arg; tail = arg; }
            else { tail->next = arg; tail = arg; }
        }

        if (cur_token_is(p, TOK_COMMA)) {
            next_token(p); // Consome vírgula e vai para o próximo arg
        } else if (cur_token_is(p, TOK_RPAREN)) {
            break; // Encontramos o fechamento, saímos do loop
        } else {
            // Se não é vírgula nem ), algo está errado
            printf("[Erro Parser L%d] Esperado ',' ou ')', encontrado %s\n", 
                p->cur_token.line, token_type_to_str(p->cur_token.type));
            return NULL;
        }
    }

    return head;
}

ASTNode* parse_expression(Parser* p) {
    ASTNode* left = NULL;

    if (cur_token_is(p, TOK_LIT_INT)) {
        char* v = rujo_strndup(p->cur_token.literal, p->cur_token.length);
        left = ast_new_literal_int(atoi(v));
        free(v);
        next_token(p);
    } else if (cur_token_is(p, TOK_LIT_STRING)) {
        char* v = rujo_strndup(p->cur_token.literal + 1, p->cur_token.length - 2);
        left = ast_new_literal_string(v);
        free(v);
        next_token(p);
    } else if (cur_token_is(p, TOK_IDENT)) {
        // Verifica se é uma chamada de função: nome(...)
        if (peek_token_is(p, TOK_LPAREN)) {
             char* name = rujo_strndup(p->cur_token.literal, p->cur_token.length);
             next_token(p); // vai para (
             ASTNode* args = parse_call_args(p);
             left = ast_new_call(name, args);
             free(name);
        } else {
            // É uma variável normal
            char* name = rujo_strndup(p->cur_token.literal, p->cur_token.length);
            left = ast_new_ident(name);
            free(name);
            next_token(p);
        }
    } else {
        return NULL;
    }

    // Acesso a membro (this.prop)
    while (cur_token_is(p, TOK_DOT)) {
        next_token(p); 
        if (!cur_token_is(p, TOK_IDENT)) break; 
        char* member = rujo_strndup(p->cur_token.literal, p->cur_token.length);
        left = ast_new_access(left, member);
        free(member);
        next_token(p);
    }

    return left;
}

ASTNode* parse_var_decl(Parser* p) {
    char* type_name = rujo_strndup(p->cur_token.literal, p->cur_token.length);
    if (!expect_peek(p, TOK_IDENT)) { free(type_name); return NULL; }
    char* var_name = rujo_strndup(p->cur_token.literal, p->cur_token.length);

    if (!expect_peek(p, TOK_ASSIGN)) { free(type_name); free(var_name); return NULL; }
    next_token(p);

    ASTNode* value = parse_expression(p);
    
    // Fallback para strings
    if (!value && cur_token_is(p, TOK_LIT_STRING)) {
         char* v = rujo_strndup(p->cur_token.literal + 1, p->cur_token.length - 2);
         value = ast_new_literal_string(v);
         free(v);
         next_token(p); 
    }

    if (cur_token_is(p, TOK_SEMICOLON)) {
        next_token(p);
    } else if (!expect_peek(p, TOK_SEMICOLON)) {
        return NULL;
    }

    return ast_new_var_decl(var_name, type_name, value);
}

ASTNode* parse_prop_decl(Parser* p) {
    next_token(p); 
    char* type_name = rujo_strndup(p->cur_token.literal, p->cur_token.length);
    if (!expect_peek(p, TOK_IDENT)) { free(type_name); return NULL; }
    char* prop_name = rujo_strndup(p->cur_token.literal, p->cur_token.length);
    if (!expect_peek(p, TOK_SEMICOLON)) { free(type_name); free(prop_name); return NULL; }
    return ast_new_prop_decl(prop_name, type_name);
}

ASTNode* parse_fn_params(Parser* p) {
    if (!cur_token_is(p, TOK_LPAREN)) return NULL;
    if (peek_token_is(p, TOK_RPAREN)) {
        next_token(p);
        return NULL;
    }
    next_token(p); 

    ASTNode* head = NULL;
    ASTNode* tail = NULL;

    while (1) {
        char* type_name = rujo_strndup(p->cur_token.literal, p->cur_token.length);
        if (!expect_peek(p, TOK_IDENT)) break;
        char* var_name = rujo_strndup(p->cur_token.literal, p->cur_token.length);

        ASTNode* param = ast_new_var_decl(var_name, type_name, NULL);
        if (!head) { head = param; tail = param; }
        else { tail->next = param; tail = param; }

        if (peek_token_is(p, TOK_COMMA)) {
            next_token(p); 
            next_token(p); 
        } else {
            break;
        }
    }
    if (!expect_peek(p, TOK_RPAREN)) return NULL;
    return head;
}

ASTNode* parse_block(Parser* p) {
    if (!cur_token_is(p, TOK_LBRACE)) return NULL;
    next_token(p); 

    ASTNode* head = NULL;
    ASTNode* tail = NULL;

    while (!cur_token_is(p, TOK_RBRACE) && !cur_token_is(p, TOK_EOF)) {
        ASTNode* stmt = NULL;

        if (cur_token_is(p, TOK_TYPE_INT) || cur_token_is(p, TOK_TYPE_STRING)) {
            stmt = parse_var_decl(p);
        }
        else if (cur_token_is(p, TOK_IDENT)) {
            ASTNode* expr = parse_expression(p);
            
            if (cur_token_is(p, TOK_ASSIGN)) {
                next_token(p);
                ASTNode* value = parse_expression(p);
                
                if (cur_token_is(p, TOK_SEMICOLON)) { next_token(p); } 
                else if (peek_token_is(p, TOK_SEMICOLON)) { next_token(p); next_token(p); }
                
                stmt = ast_new_assign(expr, value);
            } else {
                // Chamada solta dentro de bloco
                if (cur_token_is(p, TOK_SEMICOLON)) { next_token(p); }
                else if (peek_token_is(p, TOK_SEMICOLON)) { next_token(p); next_token(p); }
                stmt = expr;
            }
        } 
        else {
            next_token(p);
        }

        if (stmt) {
            if (!head) { head = stmt; tail = stmt; }
            else { tail->next = stmt; tail = stmt; }
        }
    }
    
    if (cur_token_is(p, TOK_RBRACE)) next_token(p);
    return ast_new_block(head);
}

ASTNode* parse_init_decl(Parser* p) {
    char* name = "init";
    char* ret_type = "void"; 
    if (!expect_peek(p, TOK_LPAREN)) return NULL;
    ASTNode* params = parse_fn_params(p); 
    if (!expect_peek(p, TOK_LBRACE)) return NULL;
    ASTNode* body = parse_block(p); 
    return ast_new_fn_decl(name, ret_type, params, body);
}

ASTNode* parse_class_decl(Parser* p) {
    if (!expect_peek(p, TOK_IDENT)) return NULL;
    char* class_name = rujo_strndup(p->cur_token.literal, p->cur_token.length);
    if (!expect_peek(p, TOK_LBRACE)) { free(class_name); return NULL; }
    next_token(p); 

    ASTNode* members_head = NULL;
    ASTNode* members_tail = NULL;

    while (!cur_token_is(p, TOK_RBRACE) && !cur_token_is(p, TOK_EOF)) {
        ASTNode* member = NULL;
        if (cur_token_is(p, TOK_PROP)) member = parse_prop_decl(p);
        else if (cur_token_is(p, TOK_INIT)) member = parse_init_decl(p);
        else next_token(p); 

        if (member) {
            if (!members_head) { members_head = member; members_tail = member; }
            else { members_tail->next = member; members_tail = member; }
        }
        if (member) next_token(p); // Garante avanço
    }
    return ast_new_class_decl(class_name, members_head);
}

ASTNode* parse_statement(Parser* p) {
    if (cur_token_is(p, TOK_CLASS)) return parse_class_decl(p);
    if (cur_token_is(p, TOK_TYPE_INT) || cur_token_is(p, TOK_TYPE_STRING)) return parse_var_decl(p);

    if (cur_token_is(p, TOK_IDENT)) {
        ASTNode* expr = parse_expression(p);
        
        if (cur_token_is(p, TOK_ASSIGN)) {
            next_token(p); 
            ASTNode* value = parse_expression(p);
            
            if (cur_token_is(p, TOK_SEMICOLON)) { next_token(p); } 
            else if (peek_token_is(p, TOK_SEMICOLON)) { next_token(p); next_token(p); }
            
            return ast_new_assign(expr, value);
        } else {
            // Statement Expression (ex: print)
            if (cur_token_is(p, TOK_SEMICOLON)) { next_token(p); } 
            else if (peek_token_is(p, TOK_SEMICOLON)) { next_token(p); next_token(p); }
            return expr; 
        }
    }
    next_token(p);
    return NULL;
}

ASTNode* parser_parse_program(Parser* p) {
    ASTNode* head = NULL;
    ASTNode* tail = NULL;
    while (p->cur_token.type != TOK_EOF) {
        ASTNode* stmt = parse_statement(p);
        if (stmt) {
            if (head == NULL) { head = stmt; tail = stmt; }
            else { tail->next = stmt; tail = stmt; }
        } else {
            if (p->cur_token.type != TOK_EOF) next_token(p);
        }
    }
    return ast_new_program(head);
}