#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "utils.h" 

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: rujo <comando> <arquivo.rj>\n");
        printf("Comandos:\n");
        printf("  build   Compila para executavel nativo\n");
        printf("  run     Compila e executa imediatamente\n");
        return 1;
    }

    const char* command = argv[1];
    const char* filename = argv[2];

    char* source = read_file(filename);
    if (!source) return 1;

    Lexer l;
    lexer_init(&l, source);

    parser_init(&l);
    ASTNode* root = parser_parse_program(&l);

    FILE* out_file = fopen("out.c", "w");
    if (!out_file) {
        printf("Erro: Nao foi possivel criar o arquivo temporario 'out.c'\n");
        return 1;
    }
    
    codegen_generate(root, out_file);
    fclose(out_file);

    char gcc_cmd[512];
    const char* exe_name = "program.exe";
    sprintf(gcc_cmd, "gcc out.c -o %s", exe_name);
    
    int compile_status = system(gcc_cmd);
    if (compile_status != 0) {
        printf("Erro de Compilacao (GCC falhou).\n");
        return 1;
    }

    if (strcmp(command, "run") == 0) {
        char run_cmd[512];
        sprintf(run_cmd, ".\\%s", exe_name); 
        system(run_cmd);
    } else if (strcmp(command, "build") == 0) {
        printf("Sucesso! Compilado para '%s'.\n", exe_name);
    } else {
        printf("Comando desconhecido: %s\n", command);
        return 1;
    }

    return 0;
}