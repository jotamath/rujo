#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "utils.h" 

// Detecta sistema operacional
#ifdef _WIN32
    #define CMD_RM "del"
    #define EXE_EXT ".exe"
    #define PATH_SEP "\\"
#else
    #define CMD_RM "rm"
    #define EXE_EXT ""
    #define PATH_SEP "/"
#endif

void print_usage() {
    printf("Uso: rujo <comando> <arquivo.rj>\n");
    printf("Comandos:\n");
    printf("  build   Compila para executavel nativo\n");
    printf("  run     Compila e executa imediatamente\n");
}

// --- NOVO: Extrai apenas o nome do arquivo, ignorando pastas ---
// Ex: "snippets/hello.rj" vira "hello.rj"
const char* get_filename_from_path(const char* path) {
    const char* f = strrchr(path, '/');  // Linux/Mac forward slash
    const char* b = strrchr(path, '\\'); // Windows backslash
    const char* last = (f > b) ? f : b;
    return last ? last + 1 : path;
}
// -------------------------------------------------------------

// Remove a extensão .rj e retorna o caminho base para o executável
char* get_basename(char* filename) {
    char* dot = strrchr(filename, '.');
    if (!dot || strcmp(dot, ".rj") != 0) return strdup("output");
    
    size_t len = dot - filename;
    char* base = (char*)malloc(len + 1);
    strncpy(base, filename, len);
    base[len] = '\0';
    return base;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage();
        return 1;
    }

    char* command = argv[1];
    char* filename = argv[2];

    // 1. Leitura do Arquivo
    char* input = read_file(filename);
    if (!input) return 1;

    // 2. Pipeline do Compilador
    Lexer l;
    lexer_init(&l, input);
    Parser p;
    parser_init(&p, &l);
    ASTNode* program = parser_parse_program(&p);

    if (!program) {
        printf("Erro fatal no parsing.\n");
        free(input);
        return 1;
    }

    // 3. Análise Semântica
    if (!semantic_analysis(program)) {
        printf("Falha na compilacao: Erros semanticos.\n");
        free(input);
        return 1;
    }

    // 4. Geração de Código Intermediário (Oculto)
    char temp_c_file[256];
    
    // CORREÇÃO AQUI: Usamos apenas o nome limpo para o arquivo temporário
    const char* clean_name = get_filename_from_path(filename);
    sprintf(temp_c_file, "_rujo_tmp_%s.c", clean_name); 

    FILE* out = fopen(temp_c_file, "w");
    if (!out) {
        printf("Erro de sistema: Nao foi possivel criar arquivo temporario: %s\n", temp_c_file);
        return 1;
    }
    codegen_generate(program, out);
    fclose(out);

    // 5. O Passo "Debaixo dos Panos" (Chamada ao GCC)
    char* base_name = get_basename(filename); // Mantemos o caminho original para o .exe (ex: snippets/hello.exe)
    char gcc_cmd[1024];
    
    // Comando: gcc _rujo_tmp_hello.rj.c -o snippets/hello.exe
    sprintf(gcc_cmd, "gcc \"%s\" -o \"%s%s\"", temp_c_file, base_name, EXE_EXT);
    
    int gcc_status = system(gcc_cmd);

    if (gcc_status != 0) {
        printf("Erro interno: O compilador C (GCC) falhou.\n");
        remove(temp_c_file); 
        return 1;
    }

    // 6. Limpeza
    remove(temp_c_file);

    // 7. Execução ou Build
    if (strcmp(command, "run") == 0) {
        char run_cmd[1024];
        
        // Verifica se o base_name já tem caminho ou precisa de ./
        // No Windows, system("snippets\hello.exe") funciona.
        // system(".\snippets\hello.exe") é mais seguro.
        
        #ifdef _WIN32
            // Troca barras normais por invertidas para execução segura no Windows
            for(int i=0; base_name[i]; i++) {
                if(base_name[i] == '/') base_name[i] = '\\';
            }
            sprintf(run_cmd, ".\\%s%s", base_name, EXE_EXT);
        #else
            sprintf(run_cmd, "./%s%s", base_name, EXE_EXT);
        #endif
        
        system(run_cmd);
        
    } else if (strcmp(command, "build") == 0) {
        printf("Compilacao concluida: %s%s\n", base_name, EXE_EXT);
    } else {
        printf("Comando desconhecido: %s\n", command);
        print_usage();
    }

    free(base_name);
    free(input);
    
    return 0;
}