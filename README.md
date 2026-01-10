<div align="center"><img width="300" height="300" align="center" alt="mascot" src="https://github.com/user-attachments/assets/b3b030b4-36cf-4466-99fd-90cd3a45aeea" />
</div>

# 📘 RUJO — Linguagem de Programação

> **Slogan:** Simples como Go, Segura como Rust, Legível como Python.

## 🦁 Identidade

* **Nome:** Rujo
* **Mascote:** A definir. 
* **Extensão:** `.rj`
* **Filosofia:** Previsibilidade, performance e clareza.

Rujo é uma linguagem compilada, tipada estaticamente, sem garbage collector tradicional, projetada para operar desde baixo nível até APIs backend complexas, sem runtime oculto ou comportamento imprevisível. O compilador atual (bootstrap) é escrito em **C11**.

---

## 1️⃣ Princípios Fundamentais

### 1.1 Simplicidade Radical

* Poucas palavras reservadas.
* Sintaxe explícita.
* Nenhuma inferência que afete performance.
* Ponto e vírgula (`;`) obrigatório.

### 1.2 Segurança em Tempo de Compilação

* Null safety explícito.
* Ownership e borrowing.
* Sem exceções (Erros tratados como valores).

### 1.3 Performance Previsível

* Sem GC *stop-the-world*.
* Sem *stack unwinding* implícito.
* Controle de memória sem exigir gerenciamento manual.

---

## 2️⃣ Estrutura de Projeto e CLI

### 2.1 CLI Oficial

```bash
rujo build arquivo.rj  # Compila para binário nativo
rujo run arquivo.rj    # Compila e executa imediatamente

```

### 2.2 Estrutura Padrão

```text
meu_projeto/
├── rujo.toml
└── src/
    └── main.rj

```

---

## 3️⃣ Sistema de Tipos

### 3.1 Tipagem

Estática, Forte e Explícita. Suporta introspecção em tempo de compilação via `typeOf()`.

```rujo
int x = 10;
float y = 2.5;
bool ativo = true;
print(typeOf(x)); // Saída: "int"

```

### 3.2 Tipos Primitivos Implementados

| Tipo | Descrição | Status |
| --- | --- | --- |
| `int` | Inteiro (32/64 bits) | ✅ |
| `float` | Ponto flutuante | ✅ |
| `bool` | Booleano (`true`/`false`) | ✅ |
| `byte` | 8 bits (`uint8_t`) | ✅ |
| `char` | Unicode scalar (32 bits) | ✅ |
| `string` | UTF-8 (Imutável) | ✅ |
| `void` | Ausência de valor | ✅ |

### 3.3 Null Safety (Planejado)

Qualquer tentativa de usar null fora de `?` será erro de compilação.

```rujo
string titulo;        // nunca null
string? descricao;    // pode ser null

```

---

## 4️⃣ Funções e Fluxo

### 4.1 Declaração e Retorno

```rujo
fn soma(int a, int b): int {
    return a + b;
}

```

### 4.2 Loops e Condicionais

```rujo
if (x > 10) { ... } else { ... }

while (x < 100) {
    x = x + 1;
}

for (int i = 0; i < 10; i = i + 1) {
    print(i);
}

```

---

## 5️⃣ Erros (Sem Exceções)

### 5.1 Result (Planejado)

Obrigatório tratar o retorno.

```rujo
fn lerArquivo(string path): Result<string, Error> { ... }
```

---

## 6️⃣ Classes e Propriedades

### 6.1 Classes

Sem herança. Composição acima de tudo.

```rujo
class Livro {
    prop string titulo;
    prop string isbn;
}
```

---

## 🚦 Status do Desenvolvimento (Roadmap)

O compilador atual ("Rujo Bootstrap") é escrito em C. Ele transpila código Rujo para C11 e utiliza o GCC para gerar o binário final.

### ✅ Realizado (Fase 1 & 2 - Core v1.4)

* [x] **Lexer & Parser:** Análise léxica e sintática completa.
* [x] **Tipos Primitivos:** `int`, `float`, `bool`, `byte`, `char`, `string`.
* [x] **Declaração de Variáveis:** Tipagem forte.
* [x] **Funções:** Declaração, parâmetros e `return` de valores.
* [x] **Matemática:** Operadores `+`, `-`, `*`, `/` com precedência correta.
* [x] **Lógica:** Comparadores `==`, `!=`, `<`, `>`, `<=`, `>=`.
* [x] **Fluxo de Controle:**
* [x] `if` / `else`
* [x] `while`
* [x] `for` (C-Style)


* [x] **Introspecção:** `typeOf(x)` (Resolvido em compile-time).
* [x] **IO:** `print()` polimórfico (aceita qualquer primitivo).
* [x] **Comentários:** Suporte a `//`.
* [x] **CLI:** Comandos `run` e `build`.

### 🚧 Em Andamento / TODO

* [ ] **Arrays e Vetores:**
* [ ] Declaração `int[]`.
* [ ] Acesso por índice `arr[0]`.
* [ ] Alocação dinâmica.


* [ ] **Instanciação de Classes:**
* [ ] Keyword `new`.
* [ ] Construtor `init` funcional.
      
* [ ] **Input:**
* [ ] Implementar `scan` ou leitura de console.



### 🔮 Previsto

* [ ] **Generics:** `List<T>`.
* [ ] **Result Type:** Tratamento de erros `Result<T, E>`.
* [ ] **Null Safety:** Verificação estática de nulos.
* [ ] **Ownership Checker:** O grande diferencial (Borrow Checker).

---

### 🛠️ Como Compilar e Rodar

Pré-requisitos: `gcc` e `make` (ou MinGW no Windows).

1. **Compilar o Compilador Rujo:**

```bash
make
# Gera o executável rujo (ou rujo.exe)

```

2. **Rodar um script Rujo (Modo Run):**

```bash
./rujo run meu_script.rj
# Compila e executa automaticamente

```

3. **Compilar para executável (Modo Build):**

```bash
./rujo build meu_script.rj
# Gera o arquivo 'program.exe' nativo
./program.exe

```
