CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude

TARGET := minic

SOURCES := src/main.c \
           src/lexer/lexer.c \
           src/lexer/token.c \
           src/parser/parser.c \
           src/ast/ast.c

.PHONY: all clean test test-lexer test-custom test-all

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

test: $(TARGET)
	bash tests/public/run_public_tests.sh ./$(TARGET)

test-lexer:
	$(CC) $(CFLAGS) \
		tests/lexer_driver.c \
		src/lexer/lexer.c \
		src/lexer/token.c \
		-o lexer_test
	bash tests/lexer_public/run_public_tests.sh ./lexer_test

test-custom: $(TARGET)
	bash tests/parser_custom/run_custom_tests.sh ./$(TARGET)

test-ast: $(TARGET)
	bash tests/ast_custom/run_ast_tests.sh ./$(TARGET)

test-all: test test-custom test-lexer test-ast

clean:
	rm -f $(TARGET) lexer_test parser_test syntax_test tests/actual.out