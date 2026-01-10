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

int main() {
RUJO_PRINT("--- Testando WHILE ---");
int contador = 0;
while ((contador < 3)) {
RUJO_PRINT("Contagem While:");
RUJO_PRINT(contador);
contador = (contador + 1);
}
RUJO_PRINT("--- Testando FOR ---");
for (int i = 0;
 (i < 5); i = (i + 1)) {
RUJO_PRINT("Iteracao For:");
RUJO_PRINT(i);
}
RUJO_PRINT("--- Teste de Logica Complexa ---");
int soma = 0;
for (int j = 0;
 (j < 5); j = (j + 1)) {
soma = (soma + j);
}
RUJO_PRINT("A soma de 0 a 4 deve ser 10. Resultado:");
RUJO_PRINT(soma);
if ((soma == 10)) {
RUJO_PRINT("Sucesso Total nos Loops!");
}
    return 0;
}
