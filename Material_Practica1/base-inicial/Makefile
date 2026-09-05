CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude

TARGET := minic

SOURCES := src/main.c \
           src/lexer/lexer.c \
           src/lexer/token.c

HEADERS := include/lexer/lexer.h \
           include/lexer/token.h

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

test: $(TARGET)
	@set -e; \
	trap 'rm -f tests/actual.out' 0; \
	for suite in public custom; do \
		for input in tests/$$suite/inputs/*.mc; do \
			name=$$(basename "$$input" .mc); \
			./$(TARGET) "$$input" > tests/actual.out; \
			diff -u "tests/$$suite/expected/$$name.out" tests/actual.out; \
			echo "$$suite/$$name: OK"; \
		done; \
	done

clean:
	rm -f $(TARGET) tests/actual.out