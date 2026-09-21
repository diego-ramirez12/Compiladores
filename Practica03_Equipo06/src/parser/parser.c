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
 * Reporta en stderr un token ERROR producido por el lexer.
 * El token ya fue reconocido correctamente por el lexer como un error
 * léxico; esta función únicamente muestra su ubicación y lexema.
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
 * Registra y reporta un error sintáctico asociado al token actual.
 * Activa had_error para indicar que el programa no es sintácticamente
 * válido y panic_mode para evitar diagnósticos repetidos mientras el
 * parser intenta encontrar un punto seguro de recuperación.
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
 * Recupera el análisis después de un error sintáctico.
 * Descarta tokens hasta encontrar un punto desde el cual sea razonable
 * continuar: el final de una sentencia, el inicio de una nueva sentencia
 * o un token que deba ser manejado por una estructura exterior.
 */
static void synchronize(Parser *parser)
{
    if (parser == NULL)
        return;

    while (parser->has_current &&
           !parser_check(parser, TOKEN_EOF))
    {
        /*
         * Un ';' marca el final de una sentencia. Se consume para
         * reanudar el análisis desde el token siguiente.
         */
        if (parser_check(parser, SEMICOLON))
        {
            if (!parser_advance(parser))
                return;

            parser->panic_mode = 0;
            return;
        }

        /*
         * Estos tokens pueden iniciar una nueva sentencia. Se conservan
         * como lookahead para que parse_statement pueda procesarlos.
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
         * RBRACE y ELSE pueden cerrar o continuar una estructura exterior.
         * Se conservan para que la función que contiene el contexto actual
         * pueda procesarlos.
         */
        if (parser_check(parser, RBRACE) ||
            parser_check(parser, ELSE))
        {
            parser->panic_mode = 0;
            return;
        }

        /*
         * Si el token actual no es un punto de sincronización, se descarta
         * y se continúa buscando un lugar seguro para reanudar el análisis.
         */
        if (!parser_advance(parser))
            return;
    }

    parser->panic_mode = 0;
}

/*
 * Verifica que el lookahead tenga el tipo esperado.
 * Si coincide, consume el token y devuelve 1. Si no coincide, reporta
 * el error sintáctico, ejecuta la recuperación y devuelve 0.
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
 * Analiza una expresión completa.
 * expression ::= or_expression
 * Devuelve 1 si la expresión es válida y 0 si se detecta un error.
 */
static int parse_expression(Parser *parser)
{
    return parse_or_expression(parser);
}

/*
 * Analiza operadores OR respetando la precedencia de las expresiones AND.
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
 * Analiza operadores AND respetando la precedencia de las expresiones
 * de igualdad.
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
 * Analiza comparaciones de igualdad y desigualdad.
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
 * Analiza operadores relacionales sobre expresiones aditivas.
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
 * Analiza sumas y restas sobre expresiones multiplicativas.
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
 * Analiza multiplicaciones y divisiones sobre expresiones unarias.
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
 * Analiza el operador unario '-' o delega el análisis a una expresión
 * primaria.
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
 * Analiza los elementos básicos que pueden formar una expresión.
 * primary ::= INTEGER
 *           | TRUE
 *           | FALSE
 *           | IDENTIFIER
 *           | LPAREN expression RPAREN
 * Los paréntesis permiten agrupar una expresión completa y requieren
 * obligatoriamente su RPAREN de cierre.
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
 * Analiza una declaración de variable.
 * declaration ::= type IDENTIFIER
 *                 ( ASSIGN expression )?
 *                 SEMICOLON
 * El tipo ya fue identificado por parse_statement. La declaración puede
 * incluir una inicialización y siempre debe terminar con ';'.
 */
static void parse_declaration(Parser *parser)
{
    /*
     * El lookahead es INT o BOOL. Se consume el tipo para continuar con
     * el identificador declarado.
     */
    if (!parser_advance(parser))
        return;

    /*
     * Después del tipo se requiere obligatoriamente un identificador.
     */
    if (!parser_consume(
            parser,
            IDENTIFIER,
            "un identificador"))
    {
        return;
    }

    /*
     * Si aparece ASSIGN, se analiza la expresión utilizada para
     * inicializar la variable.
     */
    if (parser_match(parser, ASSIGN))
    {
        if (!parse_expression(parser))
            return;
    }

    /*
     * Toda declaración debe finalizar con un punto y coma.
     */
    parser_consume(
        parser,
        SEMICOLON,
        "';'"
    );
}

/*
 * Analiza una asignación.
 * assignment ::= IDENTIFIER ASSIGN expression SEMICOLON
 * El identificador inicial ya fue reconocido por parse_statement.
 */
static void parse_assignment(Parser *parser)
{
    /*
     * Se consume el identificador que aparece al inicio de la asignación.
     */
    if (!parser_advance(parser))
        return;

    /*
     * Después del identificador se requiere el operador de asignación '='.
     */
    if (!parser_consume(
            parser,
            ASSIGN,
            "'='"))
    {
        return;
    }

    /*
     * Se analiza la expresión que proporciona el valor asignado.
     */
    if (!parse_expression(parser))
        return;

    /*
     * La asignación debe terminar con un punto y coma.
     */
    parser_consume(
        parser,
        SEMICOLON,
        "';'"
    );
}

/*
 * Analiza un bloque de sentencias delimitado por llaves.
 * block ::= LBRACE statement* RBRACE
 */
static void parse_block(Parser *parser)
{
    /*
     * El lookahead es LBRACE; se consume la llave de apertura del bloque.
     */
    if (!parser_advance(parser))
        return;

    /*
     * Se procesan sentencias hasta encontrar la llave de cierre.
     * También se detiene el recorrido ante ELSE para conservarlo como
     * parte de una estructura if exterior cuando falta una llave de cierre.
     * TOKEN_EOF evita continuar más allá del final de la entrada.
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
     * El bloque debe finalizar con una llave de cierre '}'.
     */
    parser_consume(
        parser,
        RBRACE,
        "'}'"
    );
}

/*
 * Analiza una sentencia while.
 * while_statement ::= WHILE LPAREN expression RPAREN statement
 * El cuerpo corresponde a una sola sentencia, que también puede ser
 * un bloque.
 */
static void parse_while_statement(Parser *parser)
{
    /*
     * Se consume el token WHILE que inicia la sentencia.
     */
    if (!parser_advance(parser))
        return;

    /*
     * La condición debe comenzar con un paréntesis de apertura.
     */
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return;
    }

    /*
     * Se analiza la expresión utilizada como condición del while.
     */
    if (!parse_expression(parser))
        return;

    /*
     * La condición debe finalizar con un paréntesis de cierre.
     */
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        return;
    }

    /*
     * Se analiza la sentencia que constituye el cuerpo del while.
     * Un bloque cuenta como una sola sentencia.
     */
    parse_statement(parser);
}

/*
 * Analiza una sentencia if con una rama else opcional.
 * if_statement ::= IF LPAREN expression RPAREN statement
 *                  ( ELSE statement )?
 */
static void parse_if_statement(Parser *parser)
{
    /*
     * Se consume el token IF que inicia la sentencia.
     */
    if (!parser_advance(parser))
        return;

    /*
     * La condición debe comenzar con un paréntesis de apertura.
     */
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return;
    }

    /*
     * Se analiza la expresión utilizada como condición del if.
     */
    if (!parse_expression(parser))
        return;

    /*
     * La condición debe finalizar con un paréntesis de cierre.
     */
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        return;
    }

    /*
     * Se analiza la sentencia asociada a la rama principal del if.
     */
    parse_statement(parser);

    if (parser->internal_failure)
        return;

    /*
     * Si el siguiente token es ELSE, se consume y se analiza la sentencia
     * correspondiente a la rama alternativa.
     */
    if (parser_match(parser, ELSE))
    {
        parse_statement(parser);
    }
}

/*
 * Analiza una sentencia print.
 * print_statement ::= PRINT LPAREN expression RPAREN SEMICOLON
 * La expresión es obligatoria, por lo que una llamada print() vacía
 * produce un error sintáctico.
 */
static void parse_print_statement(Parser *parser)
{
    /*
     * Se consume el token PRINT que inicia la sentencia.
     */
    if (!parser_advance(parser))
        return;

    /*
     * Después de PRINT se requiere un paréntesis de apertura.
     */
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return;
    }

    /*
     * Se analiza la expresión que será impresa. No se permite una
     * expresión vacía.
     */
    if (!parse_expression(parser))
        return;

    /*
     * La expresión de PRINT debe cerrarse con ')'.
     */
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        return;
    }

    /*
     * La sentencia PRINT debe finalizar con un punto y coma.
     */
    parser_consume(
        parser,
        SEMICOLON,
        "';'"
    );
}

/*
 * Determina y analiza la sentencia indicada por el lookahead actual.
 * statement ::= declaration
 *             | assignment
 *             | print_statement
 *             | if_statement
 *             | while_statement
 *             | block
 *
 * Si el token no puede iniciar ninguna sentencia válida, reporta el error
 * y utiliza la estrategia de sincronización para continuar el análisis.
 */
static void parse_statement(Parser *parser)
{
    /*
     * INT y BOOL identifican el inicio de una declaración.
     */
    if (parser_check(parser, INT) ||
        parser_check(parser, BOOL))
    {
        parse_declaration(parser);
        return;
    }

    /*
     * Un IDENTIFIER al inicio de una sentencia se interpreta como
     * una asignación.
     */
    if (parser_check(parser, IDENTIFIER))
    {
        parse_assignment(parser);
        return;
    }

    /*
     * PRINT identifica el inicio de una sentencia de impresión.
     */
    if (parser_check(parser, PRINT))
    {
        parse_print_statement(parser);
        return;
    }

    /*
     * IF identifica el inicio de una sentencia condicional.
     */
    if (parser_check(parser, IF))
    {
        parse_if_statement(parser);
        return;
    }

    /*
     * WHILE identifica el inicio de una sentencia iterativa.
     */
    if (parser_check(parser, WHILE))
    {
        parse_while_statement(parser);
        return;
    }

    /*
     * LBRACE identifica el inicio de un bloque.
     */
    if (parser_check(parser, LBRACE))
    {
        parse_block(parser);
        return;
    }

    /*
     * Cualquier otro token en esta posición no puede iniciar una sentencia.
     */
    syntax_error(
        parser,
        parser_current(parser),
        "una sentencia"
    );

    /*
     * Se consume al menos un token antes de sincronizar para garantizar
     * progreso y evitar que tokens inesperados, como un ELSE aislado o una
     * llave fuera de contexto, provoquen un ciclo sin avance.
     */
    if (!parser_advance(parser))
        return;

    synchronize(parser);
}

/*
 * Inicializa el estado del parser y obtiene el primer token de lookahead.
 * Devuelve 1 cuando la inicialización y el primer avance son correctos;
 * devuelve 0 si los argumentos son inválidos o no puede obtenerse el token.
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
     * El parser mantiene siempre un token de anticipación disponible
     * mientras no ocurra un fallo interno.
     */
    if (!parser_advance(parser))
        return 0;

    return 1;
}


/*
 * Libera el lookahead actual, solicita el siguiente token al lexer y lo
 * deja disponible para el parser.
 * Los tokens ERROR representan errores léxicos de la entrada. Se reportan
 * y descartan aquí para impedir que también generen un diagnóstico
 * sintáctico sobre el mismo carácter.
 * Devuelve 1 cuando se obtiene un token utilizable y 0 ante un fallo
 * interno del lexer o argumentos inválidos.
 */
int parser_advance(Parser *parser)
{
    LexerStatus status;

    if (parser == NULL || parser->lexer == NULL)
        return 0;

    /*
     * Antes de obtener un nuevo lookahead se libera el lexema del token
     * actual, ya que su almacenamiento pertenece al parser.
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
         * Un estado distinto de LEXER_STATUS_OK indica un fallo interno
         * al solicitar el siguiente token.
         */
        if (status != LEXER_STATUS_OK)
        {
            parser->internal_failure = 1;
            return 0;
        }

        parser->has_current = 1;

        /*
         * ERROR es un token producido válidamente por el lexer, pero
         * representa un carácter o secuencia inválida en el programa fuente.
         */
        if (parser->current.type == ERROR)
        {
            report_lexical_error(&parser->current);

            parser->had_error = 1;

            token_destroy(&parser->current);
            parser->has_current = 0;

            /*
             * El token léxico erróneo ya fue reportado y liberado. Se solicita
             * el siguiente token hasta obtener uno apto para el análisis
             * sintáctico.
             */
            continue;
        }

        return 1;
    }
}


/*
 * Comprueba si el lookahead actual tiene el tipo indicado sin consumirlo.
 * Devuelve 1 si existe un token actual del tipo solicitado y 0 en caso
 * contrario.
 */
int parser_check(const Parser *parser, TokenType type)
{
    if (parser == NULL || !parser->has_current)
        return 0;

    return parser->current.type == type;
}


/*
 * Comprueba si el lookahead coincide con el tipo indicado y, si coincide,
 * lo consume mediante parser_advance.
 * Devuelve 1 cuando hubo coincidencia y avance; en cualquier otro caso
 * devuelve 0.
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
 * Devuelve una referencia de solo lectura al lookahead actual.
 * No consume el token. Devuelve NULL si el parser es inválido o no posee
 * un token actual.
 */
const Token *parser_current(const Parser *parser)
{
    if (parser == NULL || !parser->has_current)
        return NULL;

    return &parser->current;
}
/*
 * Analiza un programa completo.
 * program ::= statement* TOKEN_EOF
 * Procesa sentencias hasta alcanzar el final de la entrada. Devuelve 1
 * únicamente si se llega a TOKEN_EOF sin errores léxicos, sintácticos ni
 * fallos internos.
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
     * Mientras no se alcance TOKEN_EOF, cada iteración analiza una
     * sentencia completa a partir del lookahead actual.
     */
    while (!parser_check(parser, TOKEN_EOF))
    {
        parse_statement(parser);

        if (parser->internal_failure)
            return 0;

        /*
         * Como verificación defensiva, el parser debe conservar un lookahead
         * válido después de procesar cada sentencia.
         */
        if (!parser->has_current)
            return 0;
    }

    /*
     * Al alcanzar TOKEN_EOF, el resultado final depende de que no se haya
     * registrado ningún error durante todo el recorrido.
     */
    return !parser->had_error &&
           !parser->internal_failure;
}

/*
 * Libera los recursos que todavía pertenecen al parser y restablece su
 * estado interno.
 * Si existe un lookahead, destruye su lexema. El Lexer asociado no se
 * destruye porque su ciclo de vida es responsabilidad del código llamador.
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
