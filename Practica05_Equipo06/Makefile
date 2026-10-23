
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude

TARGET := minic

SOURCES := src/main.c \
           src/lexer/lexer.c \
           src/lexer/token.c \
           src/parser/parser.c \
           src/ast/ast.c \
           src/semantic/symbol.c \
           src/semantic/scope.c \
           src/semantic/symbol_table.c \
           src/semantic/semantic.c

.PHONY: all clean test test-lexer test-all test-unit test-semantic-custom

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

test: $(TARGET)
	sh tests/public/run_public_tests.sh ./$(TARGET)

test-lexer:
	$(CC) $(CFLAGS) \
		tests/lexer_driver.c \
		src/lexer/lexer.c \
		src/lexer/token.c \
		-o lexer_test
	sh tests/lexer_public/run_public_tests.sh ./lexer_test

test-unit:
	$(CC) $(CFLAGS) \
		src/semantic/symbol.c \
		tests/symbol_unit.c \
		-o /tmp/minic_symbol_unit
	/tmp/minic_symbol_unit

	$(CC) $(CFLAGS) \
		src/semantic/symbol.c \
		src/semantic/scope.c \
		tests/scope_unit.c \
		-o /tmp/minic_scope_unit
	/tmp/minic_scope_unit

	$(CC) $(CFLAGS) \
		src/semantic/symbol.c \
		src/semantic/scope.c \
		src/semantic/symbol_table.c \
		tests/symbol_table_unit.c \
		-o /tmp/minic_symbol_table_unit
	/tmp/minic_symbol_table_unit

	$(CC) $(CFLAGS) \
		src/semantic/symbol.c \
		src/semantic/scope.c \
		src/semantic/symbol_table.c \
		src/semantic/semantic.c \
		tests/semantic_unit.c \
		-o /tmp/minic_semantic_unit
	/tmp/minic_semantic_unit

test-semantic-custom: $(TARGET)
	bash tests/semantic_custom/run_custom_tests.sh ./$(TARGET)

test-all: test test-lexer test-unit test-semantic-custom

clean:
	rm -f $(TARGET) lexer_test parser_test syntax_test tests/actual.out
