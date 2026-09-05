#include "lexer/lexer.h"
#include <stdlib.h>
#include <stdio.h>

int simple_token_type(int c, TokenType *type) {
    if (type == NULL) {
        return 0;
    }

    switch (c) {
        case '+':
            *type = PLUS;
            return 1;

        /* TODO: agregar los demás símbolos simples de la práctica. */
        case '-':
            *type = MINUS;
            return 1;

        case '*':
            *type = STAR;
            return 1;

        case '/':
            *type = SLASH;
            return 1;

        case '=':
            *type = ASSIGN;
            return 1;

        case '<':
            *type = LESS;
            return 1;

        case '>':
            *type = GREATER;
            return 1;

        case '(':
            *type = LPAREN;
            return 1;

        case ')':
            *type = RPAREN;
            return 1;

        case '{':
            *type = LBRACE;
            return 1;

        case '}':
            *type = RBRACE;
            return 1;

         case ';':
            *type = SEMICOLON;
            return 1;

        default:
            return 0;
    }
}

static int is_ignored_space(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/*devuelve 1 si c esta entre 0 y 9, 0 si no es digito*/
static int is_digit(int c) {
    return c >= '0' && c <= '9';
}

/* TODO: adaptar esta lógica para \r aislado y para la secuencia \r\n. */
static void advance_position(int c, int *previous_was_cr, size_t *line, size_t *column) {
    if (c == '\r') {
        (*line)++;
        *column = 0;
        *previous_was_cr = 1;
    } else if (c == '\n') {
        if (!*previous_was_cr) {
            (*line)++;
            *column = 0;
        }

        *previous_was_cr = 0;
    } else {
        (*column)++;
        *previous_was_cr = 0;
    }
}

    


int lexer_scan(FILE *file) {
    size_t line = 1;
    size_t column = 0;
    int previous_was_cr = 0;
    int c;

    if (file == NULL) {
        return 2;
    }

    while ((c = fgetc(file)) != EOF) {
        size_t token_line = line;
        size_t token_column = column;
        TokenType type;

        advance_position(c, &previous_was_cr, &line, &column);

        if (is_ignored_space(c)) {
            continue;
        }

        if (simple_token_type(c, &type)) {
            char lexeme[2] = {(char)c, '\0'};
            Token token;

            if (!token_init(&token, type, lexeme,
                            token_line, token_column)) {
                fprintf(stderr, "Error: no se pudo reservar memoria.\n");
                return 2;
            }

            token_print(&token);
            token_destroy(&token);
            continue;
        }
    

    if (is_digit(c)) {
        size_t capacity = 16;
        size_t length = 0;
        char *lexeme = malloc(capacity);
        int next;
        Token token;
    

    if (lexeme == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        return 2;
    }

    lexeme[length++] = (char)c;

    while ((next = fgetc(file)) != EOF && is_digit(next)) {
        if (length + 1 >= capacity) {
            size_t new_capacity = capacity * 2;
            char *new_lexeme = realloc(lexeme, new_capacity);

            if (new_lexeme == NULL) {
                free(lexeme);
                fprintf(stderr,
                        "Error: no se pudo reservar memoria.\n");
                return 2;
            }

            lexeme = new_lexeme;
            capacity = new_capacity;
        }

        lexeme[length++] = (char)next;
        advance_position(next, &previous_was_cr, &line, &column);
    }

    if (next != EOF) {
        if (ungetc(next, file) == EOF) {
            free(lexeme);
            fprintf(stderr,
                    "Error: no se pudo devolver el carácter al archivo.\n");
            return 2;
        }
    } else if (ferror(file)) {
        free(lexeme);
        fprintf(stderr, "Error: no se pudo leer el archivo.\n");
        return 2;
    }

    lexeme[length] = '\0';

    if (!token_init(&token, INTEGER, lexeme,
                    token_line, token_column)) {
        free(lexeme);
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        return 2;
    }

    free(lexeme);
    token_print(&token);
    token_destroy(&token);
    continue;
}
        /*
         * TODO: reconocer enteros.
         * TODO: reemplazar este diagnóstico por un token ERROR.
         */
        {
    char lexeme[2] = {(char)c, '\0'};
    Token token;

    if (!token_init(&token, ERROR, lexeme,
                    token_line, token_column)) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        return 2;
    }

    token_print(&token);
    token_destroy(&token);
}
    }

    if (ferror(file)) {
        fprintf(stderr, "Error: no se pudo leer el archivo.\n");
        return 2;
    }

    /* TODO: generar TOKEN_EOF con la posición actual. */
    {
    Token token;

    if (!token_init(&token, TOKEN_EOF, "", line, column)) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        return 2;
    }

    token_print(&token);
    token_destroy(&token);
}
    return 0;
}


