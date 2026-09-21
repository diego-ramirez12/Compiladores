#include "lexer/lexer.h"

#include <stdlib.h>
#include <string.h>


/*
 * Determina si un caracter corresponde a un token simple
 * de un solo caracter.
 *
 * Esta funcion ahora es privada del lexer, por eso es static.
 */
static int simple_token_type(int c, TokenType *type)
{
    if (type == NULL)
        return 0;

    switch (c)
    {
    case '+':
        *type = PLUS;
        return 1;

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


/* Espacios que el lexer debe ignorar. */
static int is_ignored_space(int c)
{
    return c == ' ' ||
           c == '\t' ||
           c == '\n' ||
           c == '\r';
}


/* Devuelve 1 si c esta entre 0 y 9. */
static int is_digit(int c)
{
    return c >= '0' && c <= '9';
}


/* Devuelve 1 si c puede iniciar un identificador. */
static int is_identifier_start(int c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}


/* Devuelve 1 si c puede formar parte de un identificador. */
static int is_identifier_part(int c)
{
    return is_identifier_start(c) || is_digit(c);
}


/*
 * Determina si un identificador corresponde realmente
 * a una palabra reservada.
 */
static TokenType identifier_type(const char *lexeme)
{
    if (strcmp(lexeme, "int") == 0)
        return INT;

    if (strcmp(lexeme, "bool") == 0)
        return BOOL;

    if (strcmp(lexeme, "if") == 0)
        return IF;

    if (strcmp(lexeme, "else") == 0)
        return ELSE;

    if (strcmp(lexeme, "while") == 0)
        return WHILE;

    if (strcmp(lexeme, "print") == 0)
        return PRINT;

    if (strcmp(lexeme, "true") == 0)
        return TRUE;

    if (strcmp(lexeme, "false") == 0)
        return FALSE;

    return IDENTIFIER;
}


/*
 * Reconoce operadores formados por dos caracteres.
 */
static int compound_token_type(int first, int second, TokenType *type)
{
    if (type == NULL)
        return 0;

    if (first == '=' && second == '=')
    {
        *type = EQUAL;
        return 1;
    }

    if (first == '!' && second == '=')
    {
        *type = NOT_EQUAL;
        return 1;
    }

    if (first == '<' && second == '=')
    {
        *type = LESS_EQUAL;
        return 1;
    }

    if (first == '>' && second == '=')
    {
        *type = GREATER_EQUAL;
        return 1;
    }

    if (first == '&' && second == '&')
    {
        *type = AND;
        return 1;
    }

    if (first == '|' && second == '|')
    {
        *type = OR;
        return 1;
    }

    return 0;
}


/*
 * Actualiza la linea y columna del lexer.
 *
 * La diferencia con P02 es que line, column y previous_was_cr
 * ahora viven dentro de Lexer, porque deben conservarse entre
 * llamadas a lexer_next_token().
 */
static void advance_position(Lexer *lexer, int c)
{
    if (c == '\r')
    {
        lexer->line++;
        lexer->column = 0;
        lexer->previous_was_cr = 1;
    }
    else if (c == '\n')
    {
        if (!lexer->previous_was_cr)
        {
            lexer->line++;
            lexer->column = 0;
        }

        lexer->previous_was_cr = 0;
    }
    else
    {
        lexer->column++;
        lexer->previous_was_cr = 0;
    }
}


/*
 * Crea el token que sera entregado al parser.
 *
 * Ya no se imprime ni se destruye aqui.
 */
static LexerStatus make_token(
    Token *out,
    TokenType type,
    const char *lexeme,
    size_t line,
    size_t column)
{
    if (!token_init(out, type, lexeme, line, column))
        return LEXER_STATUS_MEMORY_ERROR;

    return LEXER_STATUS_OK;
}


/*
 * Ignora el resto de un comentario //.
 */
static LexerStatus skip_line_comment(Lexer *lexer)
{
    int c;

    while ((c = fgetc(lexer->source)) != EOF)
    {
        advance_position(lexer, c);

        if (c == '\n' || c == '\r')
            break;
    }

    if (ferror(lexer->source))
        return LEXER_STATUS_IO_ERROR;

    return LEXER_STATUS_OK;
}


/*
 * Lee un identificador completo.
 *
 * El primer caracter ya fue consumido antes de entrar.
 */
static LexerStatus read_identifier(
    Lexer *lexer,
    int first,
    size_t token_line,
    size_t token_column,
    Token *out)
{
    size_t capacity = 16;
    size_t length = 0;
    char *lexeme = malloc(capacity);
    int next;
    LexerStatus status;

    if (lexeme == NULL)
        return LEXER_STATUS_MEMORY_ERROR;

    lexeme[length++] = (char)first;

    while ((next = fgetc(lexer->source)) != EOF &&
           is_identifier_part(next))
    {
        if (length + 1 >= capacity)
        {
            size_t new_capacity = capacity * 2;
            char *new_lexeme = realloc(lexeme, new_capacity);

            if (new_lexeme == NULL)
            {
                free(lexeme);
                return LEXER_STATUS_MEMORY_ERROR;
            }

            lexeme = new_lexeme;
            capacity = new_capacity;
        }

        lexeme[length++] = (char)next;
        advance_position(lexer, next);
    }

    /*
     * Si el caracter que detuvo al identificador no es EOF,
     * lo regresamos para que sea procesado en la siguiente llamada.
     */
    if (next != EOF)
    {
        if (ungetc(next, lexer->source) == EOF)
        {
            free(lexeme);
            return LEXER_STATUS_IO_ERROR;
        }
    }
    else if (ferror(lexer->source))
    {
        free(lexeme);
        return LEXER_STATUS_IO_ERROR;
    }

    lexeme[length] = '\0';

    status = make_token(
        out,
        identifier_type(lexeme),
        lexeme,
        token_line,
        token_column
    );

    free(lexeme);

    return status;
}


/*
 * Lee un literal entero completo.
 */
static LexerStatus read_integer(
    Lexer *lexer,
    int first,
    size_t token_line,
    size_t token_column,
    Token *out)
{
    size_t capacity = 16;
    size_t length = 0;
    char *lexeme = malloc(capacity);
    int next;
    LexerStatus status;

    if (lexeme == NULL)
        return LEXER_STATUS_MEMORY_ERROR;

    lexeme[length++] = (char)first;

    while ((next = fgetc(lexer->source)) != EOF &&
           is_digit(next))
    {
        if (length + 1 >= capacity)
        {
            size_t new_capacity = capacity * 2;
            char *new_lexeme = realloc(lexeme, new_capacity);

            if (new_lexeme == NULL)
            {
                free(lexeme);
                return LEXER_STATUS_MEMORY_ERROR;
            }

            lexeme = new_lexeme;
            capacity = new_capacity;
        }

        lexeme[length++] = (char)next;
        advance_position(lexer, next);
    }

    if (next != EOF)
    {
        if (ungetc(next, lexer->source) == EOF)
        {
            free(lexeme);
            return LEXER_STATUS_IO_ERROR;
        }
    }
    else if (ferror(lexer->source))
    {
        free(lexeme);
        return LEXER_STATUS_IO_ERROR;
    }

    lexeme[length] = '\0';

    status = make_token(
        out,
        INTEGER,
        lexeme,
        token_line,
        token_column
    );

    free(lexeme);

    return status;
}


/*
 * Inicializa el estado del lexer.
 *
 * El FILE * no pertenece al lexer.
 * main sera responsable de cerrarlo.
 */
int lexer_init(Lexer *lexer, FILE *source)
{
    if (lexer == NULL || source == NULL)
        return 0;

    lexer->source = source;
    lexer->line = 1;
    lexer->column = 0;
    lexer->previous_was_cr = 0;

    return 1;
}


/*
 * Produce exactamente el siguiente token disponible.
 *
 * No imprime el token.
 * No lo destruye.
 * El receptor adquiere la propiedad del Token entregado.
 */
LexerStatus lexer_next_token(Lexer *lexer, Token *out)
{
    int c;

    if (lexer == NULL ||
        lexer->source == NULL ||
        out == NULL)
    {
        return LEXER_STATUS_IO_ERROR;
    }

    for (;;)
    {
        size_t token_line;
        size_t token_column;
        TokenType type;

        c = fgetc(lexer->source);

        /*
         * EOF real: entregamos TOKEN_EOF.
         */
        if (c == EOF)
        {
            if (ferror(lexer->source))
                return LEXER_STATUS_IO_ERROR;

            return make_token(
                out,
                TOKEN_EOF,
                "",
                lexer->line,
                lexer->column
            );
        }

        /*
         * Guardamos la posicion inicial antes de avanzar.
         */
        token_line = lexer->line;
        token_column = lexer->column;

        advance_position(lexer, c);

        /*
         * Ignoramos espacios y saltos de linea.
         */
        if (is_ignored_space(c))
            continue;

        /*
         * Identificadores y palabras reservadas.
         */
        if (is_identifier_start(c))
        {
            return read_identifier(
                lexer,
                c,
                token_line,
                token_column,
                out
            );
        }

        /*
         * Operadores que pueden estar formados por
         * uno o dos caracteres.
         */
        if (c == '=' ||
            c == '!' ||
            c == '<' ||
            c == '>' ||
            c == '&' ||
            c == '|')
        {
            int next = fgetc(lexer->source);

            if (next != EOF)
            {
                if (compound_token_type(c, next, &type))
                {
                    char lexeme[3] = {
                        (char)c,
                        (char)next,
                        '\0'
                    };

                    advance_position(lexer, next);

                    return make_token(
                        out,
                        type,
                        lexeme,
                        token_line,
                        token_column
                    );
                }

                /*
                 * No formaba operador compuesto.
                 * Regresamos el segundo caracter.
                 */
                if (ungetc(next, lexer->source) == EOF)
                    return LEXER_STATUS_IO_ERROR;
            }
            else if (ferror(lexer->source))
            {
                return LEXER_STATUS_IO_ERROR;
            }
        }

        /*
         * Comentarios de linea //.
         */
        if (c == '/')
        {
            int next = fgetc(lexer->source);

            if (next == '/')
            {
                LexerStatus status;

                advance_position(lexer, next);

                status = skip_line_comment(lexer);

                if (status != LEXER_STATUS_OK)
                    return status;

                /*
                 * El comentario no produce token.
                 * Continuamos buscando el siguiente.
                 */
                continue;
            }

            if (next != EOF)
            {
                if (ungetc(next, lexer->source) == EOF)
                    return LEXER_STATUS_IO_ERROR;
            }
            else if (ferror(lexer->source))
            {
                return LEXER_STATUS_IO_ERROR;
            }
        }

        /*
         * Tokens simples: + - * / = < > ( ) { } ;
         */
        if (simple_token_type(c, &type))
        {
            char lexeme[2] = {
                (char)c,
                '\0'
            };

            return make_token(
                out,
                type,
                lexeme,
                token_line,
                token_column
            );
        }

        /*
         * Literales enteros.
         */
        if (is_digit(c))
        {
            return read_integer(
                lexer,
                c,
                token_line,
                token_column,
                out
            );
        }

        /*
         * Cualquier otro caracter se convierte en ERROR.
         *
         * IMPORTANTE:
         * Esto sigue siendo LEXER_STATUS_OK.
         * El lexer funciono correctamente; el problema esta
         * en el programa fuente.
         */
        {
            char lexeme[2] = {
                (char)c,
                '\0'
            };

            return make_token(
                out,
                ERROR,
                lexeme,
                token_line,
                token_column
            );
        }
    }
}


/*
 * El lexer no cierra el archivo porque no es su propietario.
 * Solo deja su estado en una condicion segura.
 */
void lexer_destroy(Lexer *lexer)
{
    if (lexer == NULL)
        return;

    lexer->source = NULL;
    lexer->line = 1;
    lexer->column = 0;
    lexer->previous_was_cr = 0;
}