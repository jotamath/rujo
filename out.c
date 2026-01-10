#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void print_int(int x) { printf("%d\n", x); }
void print_float(float x) { printf("%f\n", x); }
void print_string(const char* x) { printf("%s\n", x); }
void print_bool(bool x) { printf("%s\n", x ? "true" : "false"); }

#define RUJO_PRINT(x) _Generic((x), \
    int: print_int, \
    float: print_float, \
    double: print_float, \
    bool: print_bool, \
    char*: print_string, \
    const char*: print_string \
)(x)

#define RUJO_TYPEOF(x) _Generic((x), \
    _Bool: "bool", \
    uint8_t: "byte", \
    int: "int", \
    float: "float", \
    double: "float", \
    uint32_t: "char", \
    char*: "string", \
    const char*: "string", \
    default: "unknown" \
)

int calcularDobro(int n) {
return (n * 2);
}

int somar(int a, int b) {
return (a + b);
}

int main() {
RUJO_PRINT("--- Teste de Funcoes ---");
int resultado = somar(10, 5);
RUJO_PRINT("10 + 5 e igual a (deve ser 15):");
RUJO_PRINT(resultado);
RUJO_PRINT(RUJO_TYPEOF(resultado));
RUJO_PRINT("--- Teste de Matematica ---");
int conta = (10 + (5 * 2));
if ((conta == 20)) {
RUJO_PRINT("Sucesso: Matematica funciona (Resultado = 20)");
}
 else {
RUJO_PRINT("Erro: Precedencia incorreta");
}
int complexo = (calcularDobro(5) + (100 / 2));
if ((complexo == 60)) {
RUJO_PRINT("Sucesso: Funcoes misturadas com Matematica (Resultado = 60)");
}
    return 0;
}
