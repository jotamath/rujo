# 📘 RUJO — Linguagem de Programação

> **Slogan:** Simples como Go, Segura como Rust, Legível como Python.

![Status](https://img.shields.io/badge/status-pre--alpha-orange)
![Version](https://img.shields.io/badge/version-1.4-blue)
![Build](https://img.shields.io/badge/build-passing-green)

## 🦁 Identidade

* **Nome:** Rujo
* **Mascote:** A definir
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

### 2.1 CLI Oficial (Planejado)
```bash
rujo init meu_projeto
rujo build
rujo run
rujo test

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

Estática, Forte e Explícita.

```rujo
int x = 10;
float y = 2.5;
bool ativo = true;

```

### 3.2 Tipos Primitivos

| Tipo | Descrição |
| --- | --- |
| `int` | Inteiro |
| `float` | Ponto flutuante |
| `bool` | Booleano |
| `byte` | 8 bits |
| `char` | Unicode scalar (não UTF-8) |
| `string` | UTF-8 |
| `void` | Ausência de valor |

### 3.3 String (Primitivo)

UTF-8 garantido, alocada no heap, imutável por padrão.

```rujo
string nome = "Rujo";

```

### 3.4 Null Safety

Qualquer tentativa de usar null fora de `?` é erro de compilação.

```rujo
string titulo;        // nunca null
string? descricao;    // pode ser null

```

---

## 4️⃣ Funções

### 4.1 Declaração

```rujo
fn soma(int a, int b): int {
    return a + b;
}

```

### 4.2 Funções sem Retorno

```rujo
fn log(string msg): void {
    print(msg);
}

```

---

## 5️⃣ Erros (Sem Exceções)

### 5.1 Result

Obrigatório tratar o retorno.

```rujo
fn lerArquivo(string path): Result<string, Error> { ... }

// Uso:
Result<string, Error> res = lerArquivo("dados.txt");

if res.isErr() {
    return res.err();
}
string dados = res.unwrap();

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

### 6.2 Visibilidade

`private` por padrão. `pub` torna visível externamente.

```rujo
pub class Livro {
    pub prop string titulo;
}

```

---

## 7️⃣ Inicialização de Objetos

### 7.1 Construtor Oficial — `init`

`init` é o único construtor permitido.

```rujo
class DizerOi {
    prop string mensagem;

    init(string saudacao) {
        this.mensagem = saudacao;
    }
}

// Uso:
DizerOi d = new DizerOi("Olá!");

```

---

## 8️⃣ Ownership e Borrowing (Roadmap)

Regras verificadas em compile-time (sem custo de runtime):

* Apenas um borrow mutável (`&mut`).
* Vários borrows imutáveis (`&`).

```rujo
fn processar(Livro livro);       // move
fn ler(&Livro livro);            // borrow imutável
fn editar(&mut Livro livro);     // borrow mutável

```

---

## 🚦 Status do Desenvolvimento (Roadmap)

O compilador atual ("Rujo Bootstrap") é escrito em C. Ele realiza a análise léxica, sintática, semântica e transpila o código Rujo para C11, que é então compilado pelo GCC.

### ✅ Realizado (Fase 1 & 2 - Core)

* [x] **Lexer:** Tokenização completa da sintaxe básica.
* [x] **Parser:** Análise sintática (AST) implementada.
* [x] Declaração de variáveis (`int`, `string`).
* [x] Declaração de Classes e Propriedades (`class`, `prop`).
* [x] Construtor (`init`).
* [x] Blocos de código e Escopo.
* [x] Atribuições (`x = y`) e Acesso a membros (`this.prop`).
* [x] Chamadas de Função (`print("Ola")`).


* [x] **Symbol Table:** Suporte a escopos aninhados (Global -> Classe -> Função -> Bloco).
* [x] **Semantic Analysis:**
* [x] Detecção de variáveis não declaradas.
* [x] Detecção de redeclaração de variáveis.
* [x] Verificação básica de tipos (impedir `int x = "string"`).
* [x] Validação de contexto do `this`.


* [x] **Codegen (Transpiler):** Geração de código C válido.
* [x] Structs equivalentes.
* [x] Métodos com *name mangling* (`Classe_init`).
* [x] Função `main` automática.
* [x] Mapeamento de `print` para `printf`.


* [x] **Build System:** Makefile configurado.

### 🚧 Em Andamento / TODO (Fase 3 & 4)

* [ ] **Fluxo de Controle:**
* [ ] `if` / `else`
* [ ] Loops (`while`, `for`)
* [ ] `return` statements.


* [ ] **Expressões Matemáticas:**
* [ ] Operações binárias (`+`, `-`, `*`, `/`).
* [ ] Operações lógicas (`==`, `!=`, `!`).


* [ ] **Input/Output:**
* [ ] Implementar `scan` ou leitura de input.


* [ ] **Instanciação:**
* [ ] Implementar keyword `new` para alocação de objetos.


* [ ] **Types Avançados:**
* [ ] Implementar Null Safety (`?`).
* [ ] Implementar Arrays/Listas.
* [ ] Implementar `Result<T, E>`.


* [ ] **Ownership Checker:** O grande desafio da Fase 4.

---

### 🛠️ Como Compilar e Rodar

Pré-requisitos: `gcc` e `make` (ou MinGW no Windows).

1. **Compilar o Compilador:**
```bash
make

```


2. **Compilar um arquivo .rj:**
```bash
./rujo seu_arquivo.rj
# Isso gera um arquivo out.c

```


3. **Gerar o binário final:**
```bash
gcc out.c -o programa_final
./programa_final

```



```

```