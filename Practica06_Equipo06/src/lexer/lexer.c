#include "lexer/lexer.h"

#include <stdlib.h>
#include <string.h>


/*
 * Determina si un carácter corresponde a un token simple de un solo
 * carácter.
 *
 * Si existe una correspondencia, almacena el tipo de token en *type y
 * devuelve 1. En caso contrario, devuelve 0.
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


/*
 * Indica si el carácter recibido corresponde a un espacio o salto de línea
 * que debe ser ignorado por el lexer.
 */
static int is_ignored_space(int c)
{
    return c == ' ' ||
           c == '\t' ||
           c == '\n' ||
           c == '\r';
}


/*
 * Devuelve 1 si el carácter recibido corresponde a un dígito decimal.
 */
static int is_digit(int c)
{
    return c >= '0' && c <= '9';
}


/*
 * Devuelve 1 si el carácter puede aparecer como primer carácter de un
 * identificador.
 */
static int is_identifier_start(int c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}


/*
 * Devuelve 1 si el carácter puede formar parte de un identificador después
 * de su primer carácter.
 */
static int is_identifier_part(int c)
{
    return is_identifier_start(c) || is_digit(c);
}


/*
 * Determina el tipo de token asociado a un lexema que inicialmente fue
 * reconocido como identificador.
 * Si el lexema coincide con una palabra reservada o un literal booleano,
 * devuelve su tipo específico; en cualquier otro caso devuelve IDENTIFIER.
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
 * Determina si dos caracteres consecutivos forman un operador compuesto.
 * Si existe una coincidencia, almacena el tipo correspondiente en *type y
 * devuelve 1. En caso contrario, devuelve 0.
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
 * Actualiza la línea y columna actuales del lexer después de consumir un
 * carácter.
 * También controla secuencias CRLF para evitar contar '
' y '
' como dos
 * saltos de línea independientes.
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
 * Inicializa un token con la información recibida y traduce un fallo de
 * memoria al estado correspondiente del lexer.
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
 * Consume los caracteres restantes de un comentario de línea iniciado por
 * '//', hasta encontrar un salto de línea o el final del archivo.
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
 * Lee un identificador completo a partir de un primer carácter que ya fue
 * consumido.
 * El búfer del lexema crece dinámicamente cuando es necesario. El primer
 * carácter que no pertenece al identificador se devuelve al flujo para que
 * sea procesado por la siguiente llamada al lexer.
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
     * El carácter que detuvo la lectura no pertenece al identificador.
     * Si no es EOF, se devuelve al flujo para procesarlo posteriormente.
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
 * Lee un literal entero completo a partir de un primer dígito ya consumido.
 * El búfer del lexema crece dinámicamente mientras se leen más dígitos.
 * El primer carácter que no pertenece al entero se devuelve al flujo.
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
 * Inicializa el estado del lexer sobre el flujo de entrada recibido.
 * El lexer conserva una referencia al FILE, pero no adquiere su propiedad;
 * el código que abrió el archivo sigue siendo responsable de cerrarlo.
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
 * Obtiene el siguiente token disponible desde el flujo de entrada.
 * La función ignora espacios y comentarios de línea, reconoce identificadores,
 * palabras reservadas, literales enteros, operadores y símbolos simples.
 * El Token resultante se entrega mediante out y pasa a ser responsabilidad
 * del código receptor.
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
         * Si se alcanza el final real del archivo, se produce TOKEN_EOF.
         * Un error del flujo se reporta como LEXER_STATUS_IO_ERROR.
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
         * Se conserva la posición inicial del token antes de actualizar
         * la posición global con el carácter recién consumido.
         */
        token_line = lexer->line;
        token_column = lexer->column;

        advance_position(lexer, c);

        /*
         * Los espacios, tabuladores y saltos de línea no generan tokens.
         */
        if (is_ignored_space(c))
            continue;

        /*
         * Si el carácter puede iniciar un identificador, se delega la lectura
         * del lexema completo a read_identifier.
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
         * Estos caracteres pueden iniciar operadores de uno o dos caracteres.
         * Primero se intenta reconocer la versión compuesta.
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
                 * Si el segundo carácter no completa un operador compuesto,
                 * se devuelve al flujo para que pueda procesarse después.
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
         * Un '/' puede iniciar un comentario de línea. Si no aparece un
         * segundo '/', continuará procesándose posteriormente como token SLASH.
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
                 * Los comentarios no producen tokens, por lo que se continúa
                 * buscando el siguiente elemento léxico de la entrada.
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
         * Se reconocen los símbolos de un solo carácter definidos por
         * simple_token_type.
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
         * Si el carácter actual es un dígito, se lee el literal entero
         * completo.
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
         * Cualquier carácter que no pertenezca a una categoría válida se
         * convierte en un token ERROR.
         * Esto no representa un fallo interno del lexer: el análisis léxico
         * terminó correctamente y el error pertenece al programa fuente.
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
 * Restablece el estado interno del lexer.
 * El archivo asociado no se cierra porque el lexer no es su propietario.
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