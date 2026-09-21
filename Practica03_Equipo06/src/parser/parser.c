#include "parser/parser.h"

#include <stdio.h>

static void syntax_error(
    Parser *parser,
    const Token *token,
    const char *expected
);

static void synchronize(Parser *parser);

static int parser_consume(
    Parser *parser,
    TokenType expected,
    const char *message
);

static void parse_statement(Parser *parser);

static void parse_declaration(Parser *parser);

static void parse_assignment(Parser *parser);

static void parse_print_statement(Parser *parser);

static void parse_if_statement(Parser *parser);

static void parse_while_statement(Parser *parser);

static void parse_block(Parser *parser);

static int parse_expression(Parser *parser);

static int parse_or_expression(Parser *parser);

static int parse_and_expression(Parser *parser);

static int parse_equality_expression(Parser *parser);

static int parse_comparison_expression(Parser *parser);

static int parse_additive_expression(Parser *parser);

static int parse_multiplicative_expression(Parser *parser);

static int parse_unary_expression(Parser *parser);

static int parse_primary(Parser *parser);
/*
 * Reporta un token ERROR recibido del lexer.
 *
 * Este diagnóstico es léxico, no sintáctico.
 */
static void report_lexical_error(const Token *token)
{
    if (token == NULL)
        return;

    fprintf(
        stderr,
        "Error lexico [%zu:%zu]: caracter invalido '%s'.\n",
        token->line,
        token->column,
        token->lexeme
    );
}

/*
 * Reporta un error sintactico.
 *
 * panic_mode evita reportar repetidamente el mismo
 * problema antes de que el parser se sincronice.
 */
static void syntax_error(
    Parser *parser,
    const Token *token,
    const char *expected)
{
    if (parser == NULL || token == NULL || expected == NULL)
        return;

    if (parser->panic_mode)
        return;

    parser->panic_mode = 1;
    parser->had_error = 1;

    if (token->type == TOKEN_EOF)
    {
        fprintf(
            stderr,
            "Error sintactico [%zu:%zu]: se esperaba %s, "
            "pero se encontro TOKEN_EOF.\n",
            token->line,
            token->column,
            expected
        );
    }
    else
    {
        fprintf(
            stderr,
            "Error sintactico [%zu:%zu]: se esperaba %s, "
            "pero se encontro '%s'.\n",
            token->line,
            token->column,
            expected,
            token->lexeme
        );
    }
}

/*
 * Busca un punto seguro donde reanudar el analisis
 * despues de un error sintactico.
 */
static void synchronize(Parser *parser)
{
    if (parser == NULL)
        return;

    while (parser->has_current &&
           !parser_check(parser, TOKEN_EOF))
    {
        /*
         * Si encontramos ';', lo consumimos y dejamos
         * el parser listo para la siguiente sentencia.
         */
        if (parser_check(parser, SEMICOLON))
        {
            if (!parser_advance(parser))
                return;

            parser->panic_mode = 0;
            return;
        }

        /*
         * Estos tokens pueden comenzar una nueva sentencia.
         * NO deben consumirse.
         */
        if (parser_check(parser, INT) ||
            parser_check(parser, BOOL) ||
            parser_check(parser, IF) ||
            parser_check(parser, WHILE) ||
            parser_check(parser, PRINT) ||
            parser_check(parser, IDENTIFIER) ||
            parser_check(parser, LBRACE))
        {
            parser->panic_mode = 0;
            return;
        }

        /*
         * Estos tokens pertenecen posiblemente a una
         * estructura exterior. Tampoco deben consumirse.
         */
        if (parser_check(parser, RBRACE) ||
            parser_check(parser, ELSE))
        {
            parser->panic_mode = 0;
            return;
        }

        /*
         * No encontramos nada útil todavía:
         * descartamos el token y seguimos buscando.
         */
        if (!parser_advance(parser))
            return;
    }

    parser->panic_mode = 0;
}

/*
 * Exige que el token actual tenga el tipo expected.
 *
 * Si coincide, lo consume.
 * Si no coincide, reporta el error y trata de recuperarse.
 */
static int parser_consume(
    Parser *parser,
    TokenType expected,
    const char *message)
{
    if (parser == NULL)
        return 0;

    if (parser_check(parser, expected))
    {
        if (!parser_advance(parser))
            return 0;

        return 1;
    }

    syntax_error(
        parser,
        parser_current(parser),
        message
    );

    synchronize(parser);

    return 0;
}

/*
 * expression ::= or_expression
 */
static int parse_expression(Parser *parser)
{
    return parse_or_expression(parser);
}

/*
 * or_expression ::= and_expression
 *                   ( OR and_expression )*
 */
static int parse_or_expression(Parser *parser)
{
    if (!parse_and_expression(parser))
        return 0;

    while (parser_match(parser, OR))
    {
        if (!parse_and_expression(parser))
            return 0;
    }

    return 1;
}

/*
 * and_expression ::= equality_expression
 *                    ( AND equality_expression )*
 */
static int parse_and_expression(Parser *parser)
{
    if (!parse_equality_expression(parser))
        return 0;

    while (parser_match(parser, AND))
    {
        if (!parse_equality_expression(parser))
            return 0;
    }

    return 1;
}

/*
 * equality_expression ::= comparison_expression
 *                         ( (EQUAL | NOT_EQUAL)
 *                           comparison_expression )*
 */
static int parse_equality_expression(Parser *parser)
{
    if (!parse_comparison_expression(parser))
        return 0;

    while (parser_check(parser, EQUAL) ||
           parser_check(parser, NOT_EQUAL))
    {
        if (!parser_advance(parser))
            return 0;

        if (!parse_comparison_expression(parser))
            return 0;
    }

    return 1;
}

/*
 * comparison_expression ::= additive_expression
 *                           ( (LESS
 *                            | LESS_EQUAL
 *                            | GREATER
 *                            | GREATER_EQUAL)
 *                             additive_expression )*
 */
static int parse_comparison_expression(Parser *parser)
{
    if (!parse_additive_expression(parser))
        return 0;

    while (parser_check(parser, LESS) ||
           parser_check(parser, LESS_EQUAL) ||
           parser_check(parser, GREATER) ||
           parser_check(parser, GREATER_EQUAL))
    {
        if (!parser_advance(parser))
            return 0;

        if (!parse_additive_expression(parser))
            return 0;
    }

    return 1;
}

/*
 * additive_expression ::= multiplicative_expression
 *                         ( (PLUS | MINUS)
 *                           multiplicative_expression )*
 */
static int parse_additive_expression(Parser *parser)
{
    if (!parse_multiplicative_expression(parser))
        return 0;

    while (parser_check(parser, PLUS) ||
           parser_check(parser, MINUS))
    {
        if (!parser_advance(parser))
            return 0;

        if (!parse_multiplicative_expression(parser))
            return 0;
    }

    return 1;
}

/*
 * multiplicative_expression ::= unary_expression
 *                               ( (STAR | SLASH)
 *                                 unary_expression )*
 */
static int parse_multiplicative_expression(Parser *parser)
{
    if (!parse_unary_expression(parser))
        return 0;

    while (parser_check(parser, STAR) ||
           parser_check(parser, SLASH))
    {
        if (!parser_advance(parser))
            return 0;

        if (!parse_unary_expression(parser))
            return 0;
    }

    return 1;
}

/*
 * unary_expression ::= MINUS unary_expression
 *                    | primary
 */
static int parse_unary_expression(Parser *parser)
{
    if (parser_match(parser, MINUS))
        return parse_unary_expression(parser);

    return parse_primary(parser);
}

/*
 * primary ::= INTEGER
 *           | TRUE
 *           | FALSE
 *           | IDENTIFIER
 *           | LPAREN expression RPAREN
 */
static int parse_primary(Parser *parser)
{
    if (parser_match(parser, INTEGER))
        return 1;

    if (parser_match(parser, TRUE))
        return 1;

    if (parser_match(parser, FALSE))
        return 1;

    if (parser_match(parser, IDENTIFIER))
        return 1;

    if (parser_match(parser, LPAREN))
    {
        if (!parse_expression(parser))
            return 0;

        if (!parser_consume(
                parser,
                RPAREN,
                "')'"))
        {
            return 0;
        }

        return 1;
    }

    syntax_error(
        parser,
        parser_current(parser),
        "una expresion"
    );

    synchronize(parser);

    return 0;
}

/*
 * declaration ::= type IDENTIFIER
 *                 ( ASSIGN expression )?
 *                 SEMICOLON
 */
static void parse_declaration(Parser *parser)
{
    /*
     * parse_statement ya comprobo que estamos
     * sobre INT o BOOL.
     */
    if (!parser_advance(parser))
        return;

    /*
     * IDENTIFIER obligatorio.
     */
    if (!parser_consume(
            parser,
            IDENTIFIER,
            "un identificador"))
    {
        return;
    }

    /*
     * Inicializacion opcional:
     *
     * ( ASSIGN expression )?
     */
    if (parser_match(parser, ASSIGN))
    {
        if (!parse_expression(parser))
            return;
    }

    /*
     * Toda declaracion termina obligatoriamente en ';'.
     */
    parser_consume(
        parser,
        SEMICOLON,
        "';'"
    );
}

/*
 * assignment ::= IDENTIFIER ASSIGN expression SEMICOLON
 */
static void parse_assignment(Parser *parser)
{
    /*
     * parse_statement ya comprobo que el token actual
     * es IDENTIFIER.
     */
    if (!parser_advance(parser))
        return;

    /*
     * ASSIGN obligatorio.
     */
    if (!parser_consume(
            parser,
            ASSIGN,
            "'='"))
    {
        return;
    }

    /*
     * expression obligatoria.
     */
    if (!parse_expression(parser))
        return;

    /*
     * SEMICOLON obligatorio.
     */
    parser_consume(
        parser,
        SEMICOLON,
        "';'"
    );
}

/*
 * block ::= LBRACE statement* RBRACE
 */
static void parse_block(Parser *parser)
{
    /*
     * parse_statement ya comprobo LBRACE.
     */
    if (!parser_advance(parser))
        return;

    /*
     * statement*
     *
     * Seguimos hasta cerrar el bloque.
     *
     * También nos detenemos ante ELSE para que,
     * si falta una llave de cierre, el ELSE quede
     * disponible para la estructura exterior.
     */
    while (!parser_check(parser, RBRACE) &&
           !parser_check(parser, ELSE) &&
           !parser_check(parser, TOKEN_EOF))
    {
        parse_statement(parser);

        if (parser->internal_failure)
            return;
    }

    /*
     * '}' obligatorio.
     */
    parser_consume(
        parser,
        RBRACE,
        "'}'"
    );
}

/*
 * while_statement ::= WHILE LPAREN expression RPAREN statement
 */
static void parse_while_statement(Parser *parser)
{
    /*
     * Consumimos WHILE.
     */
    if (!parser_advance(parser))
        return;

    /*
     * '(' obligatorio.
     */
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return;
    }

    /*
     * Condicion obligatoria.
     */
    if (!parse_expression(parser))
        return;

    /*
     * ')' obligatorio.
     */
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        return;
    }

    /*
     * El cuerpo es UNA sentencia.
     *
     * Esa sentencia puede ser un bloque.
     */
    parse_statement(parser);
}

/*
 * if_statement ::= IF LPAREN expression RPAREN statement
 *                  ( ELSE statement )?
 */
static void parse_if_statement(Parser *parser)
{
    /*
     * Consumimos IF.
     */
    if (!parser_advance(parser))
        return;

    /*
     * '(' obligatorio.
     */
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return;
    }

    /*
     * Condicion obligatoria.
     */
    if (!parse_expression(parser))
        return;

    /*
     * ')' obligatorio.
     */
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        return;
    }

    /*
     * Cuerpo del IF.
     */
    parse_statement(parser);

    if (parser->internal_failure)
        return;

    /*
     * ELSE opcional.
     */
    if (parser_match(parser, ELSE))
    {
        parse_statement(parser);
    }
}

/*
 * print_statement ::= PRINT LPAREN expression RPAREN SEMICOLON
 */
static void parse_print_statement(Parser *parser)
{
    /*
     * parse_statement ya comprobo PRINT.
     */
    if (!parser_advance(parser))
        return;

    /*
     * '(' obligatorio.
     */
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return;
    }

    /*
     * expression obligatoria.
     *
     * Por eso print(); es invalido.
     */
    if (!parse_expression(parser))
        return;

    /*
     * ')' obligatorio.
     */
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        return;
    }

    /*
     * ';' obligatorio.
     */
    parser_consume(
        parser,
        SEMICOLON,
        "';'"
    );
}

/*
 * statement ::= declaration
 *             | assignment
 *             | print_statement
 *             | if_statement
 *             | while_statement
 *             | block
 */
static void parse_statement(Parser *parser)
{
    /*
     * declaration
     */
    if (parser_check(parser, INT) ||
        parser_check(parser, BOOL))
    {
        parse_declaration(parser);
        return;
    }

    /*
     * assignment
     */
    if (parser_check(parser, IDENTIFIER))
    {
        parse_assignment(parser);
        return;
    }

    /*
     * print_statement
     */
    if (parser_check(parser, PRINT))
    {
        parse_print_statement(parser);
        return;
    }

    /*
     * if_statement
     */
    if (parser_check(parser, IF))
    {
        parse_if_statement(parser);
        return;
    }

    /*
     * while_statement
     */
    if (parser_check(parser, WHILE))
    {
        parse_while_statement(parser);
        return;
    }

    /*
     * block
     */
    if (parser_check(parser, LBRACE))
    {
        parse_block(parser);
        return;
    }

    /*
     * Token inesperado.
     */
    syntax_error(
        parser,
        parser_current(parser),
        "una sentencia"
    );

    /*
     * Garantizamos progreso.
     *
     * Esto es especialmente importante para un ELSE
     * sin IF o una llave inesperada a nivel superior.
     */
    if (!parser_advance(parser))
        return;

    synchronize(parser);
}

/*
 * Inicializa el parser y solicita inmediatamente
 * el primer token de anticipación.
 */
int parser_init(Parser *parser, Lexer *lexer)
{
    if (parser == NULL || lexer == NULL)
        return 0;

    parser->lexer = lexer;

    parser->has_current = 0;
    parser->had_error = 0;
    parser->panic_mode = 0;
    parser->internal_failure = 0;

    /*
     * El parser comienza con un lookahead disponible.
     */
    if (!parser_advance(parser))
        return 0;

    return 1;
}


/*
 * Descarta el token actual y obtiene el siguiente.
 *
 * Los Token ERROR se reportan aquí y se consumen,
 * para impedir que posteriormente produzcan además
 * un error sintáctico.
 */
int parser_advance(Parser *parser)
{
    LexerStatus status;

    if (parser == NULL || parser->lexer == NULL)
        return 0;

    /*
     * Si ya poseíamos un token, primero liberamos
     * su lexema.
     */
    if (parser->has_current)
    {
        token_destroy(&parser->current);
        parser->has_current = 0;
    }

    for (;;)
    {
        status = lexer_next_token(
            parser->lexer,
            &parser->current
        );

        /*
         * Fallo interno del lexer.
         */
        if (status != LEXER_STATUS_OK)
        {
            parser->internal_failure = 1;
            return 0;
        }

        parser->has_current = 1;

        /*
         * ERROR es un token válido producido correctamente
         * por el lexer, pero representa un error del programa.
         */
        if (parser->current.type == ERROR)
        {
            report_lexical_error(&parser->current);

            parser->had_error = 1;

            token_destroy(&parser->current);
            parser->has_current = 0;

            /*
             * Seguimos solicitando tokens hasta encontrar
             * uno que el parser sintáctico pueda analizar.
             */
            continue;
        }

        return 1;
    }
}


/*
 * Consulta si el lookahead es del tipo indicado.
 *
 * No consume ningún token.
 */
int parser_check(const Parser *parser, TokenType type)
{
    if (parser == NULL || !parser->has_current)
        return 0;

    return parser->current.type == type;
}


/*
 * Si el token actual coincide con type, lo consume
 * y avanza al siguiente.
 */
int parser_match(Parser *parser, TokenType type)
{
    if (!parser_check(parser, type))
        return 0;

    if (!parser_advance(parser))
        return 0;

    return 1;
}


/*
 * Permite consultar el token actual sin consumirlo.
 */
const Token *parser_current(const Parser *parser)
{
    if (parser == NULL || !parser->has_current)
        return NULL;

    return &parser->current;
}
/*
 * program ::= statement* TOKEN_EOF
 */
int parser_parse(Parser *parser)
{
    if (parser == NULL ||
        !parser->has_current ||
        parser->internal_failure)
    {
        return 0;
    }

    /*
     * statement*
     *
     * Seguimos hasta encontrar el final real
     * del archivo.
     */
    while (!parser_check(parser, TOKEN_EOF))
    {
        parse_statement(parser);

        if (parser->internal_failure)
            return 0;

        /*
         * Defensa adicional:
         * parse_statement debe siempre avanzar o
         * dejar un token que pueda ser procesado.
         */
        if (!parser->has_current)
            return 0;
    }

    /*
     * Llegar aqui significa que verificamos TOKEN_EOF.
     *
     * El programa solamente es valido si durante
     * todo el recorrido no hubo errores.
     */
    return !parser->had_error &&
           !parser->internal_failure;
}

/*
 * Libera el último token propiedad del parser.
 *
 * El parser NO destruye el lexer.
 */
void parser_destroy(Parser *parser)
{
    if (parser == NULL)
        return;

    if (parser->has_current)
    {
        token_destroy(&parser->current);
        parser->has_current = 0;
    }

    parser->lexer = NULL;

    parser->had_error = 0;
    parser->panic_mode = 0;
    parser->internal_failure = 0;
}
