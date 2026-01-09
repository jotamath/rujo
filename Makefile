CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./src

# Lista explícita de todos os arquivos fonte
SRC = src/main.c src/lexer.c src/utils.c src/ast.c src/parser.c src/symbol_table.c src/semantic.c src/codegen.c

# Gera a lista de objetos (.o) substituindo .c por .o na lista SRC
OBJ = $(SRC:.c=.o)

TARGET = rujo

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) $(TARGET).exe

run: all
	./$(TARGET)