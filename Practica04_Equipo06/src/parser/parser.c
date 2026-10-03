#include "parser/parser.h"

#include "ast/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *text)
{
    if (text == NULL)
        return NULL;
    size_t length = strlen(text) + 1;
    char *copy = malloc(length);
    if (copy == NULL)
        return NULL;
    memcpy(copy, text, length);
    return copy;
}
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
static ASTNode *parse_statement(Parser *parser);
static ASTNode *parse_declaration(Parser *parser);
static ASTNode *parse_assignment(Parser *parser);
static ASTNode *parse_print_statement(Parser *parser);
static ASTNode *parse_if_statement(Parser *parser);
static ASTNode *parse_while_statement(Parser *parser);
static ASTNode *parse_block(Parser *parser);
static ASTNode *parse_expression(Parser *parser);
static ASTNode *parse_or_expression(Parser *parser);
static ASTNode *parse_and_expression(Parser *parser);
static ASTNode *parse_equality_expression(Parser *parser);
static ASTNode *parse_comparison_expression(Parser *parser);
static ASTNode *parse_additive_expression(Parser *parser);
static ASTNode *parse_multiplicative_expression(Parser *parser);
static ASTNode *parse_unary_expression(Parser *parser);
static ASTNode *parse_primary(Parser *parser);

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
 * Analiza una expresión completa respetando todos los niveles de precedencia.
 * expression ::= or_expression
 * Devuelve el nodo raíz de la expresión construida o NULL si ocurre un error.
 */

static ASTNode *parse_expression(Parser *parser)
{
    return parse_or_expression(parser);
}

/*
 * Analiza expresiones con el operador lógico OR.
 * Primero procesa expresiones AND para respetar su mayor precedencia y
 * construye nodos binarios OP_OR de izquierda a derecha.
 * or_expression ::= and_expression
 *                   ( OR and_expression )*
 */

static ASTNode *parse_or_expression(Parser *parser)
{
    ASTNode *left =
        parse_and_expression(parser);
    if (left == NULL)
        return NULL;
    while (parser_check(parser, OR))
    {
        const Token *token = parser_current(parser);
        int line = (int)token->line;
        int column = (int)token->column;
        if (!parser_advance(parser))
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *right =
            parse_and_expression(parser);
        if (right == NULL)
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *node = ast_create_binary(
            OP_OR,
            left,
            right,
            line,
            column
        );
        if (node == NULL)
        {
            ast_destroy(left);
            ast_destroy(right);
            parser->internal_failure = 1;
            return NULL;
        }
        left = node;
    }
    return left;
}

/*
 * Analiza expresiones con el operador lógico AND.
 * Primero procesa expresiones de igualdad para respetar su mayor precedencia
 * y construye nodos binarios OP_AND de izquierda a derecha.
 * and_expression ::= equality_expression
 *                    ( AND equality_expression )*
 */

static ASTNode *parse_and_expression(Parser *parser)
{
    ASTNode *left =
        parse_equality_expression(parser);
    if (left == NULL)
        return NULL;
    while (parser_check(parser, AND))
    {
        const Token *token = parser_current(parser);
        int line = (int)token->line;
        int column = (int)token->column;
        if (!parser_advance(parser))
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *right =
            parse_equality_expression(parser);
        if (right == NULL)
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *node = ast_create_binary(
            OP_AND,
            left,
            right,
            line,
            column
        );
        if (node == NULL)
        {
            ast_destroy(left);
            ast_destroy(right);
            parser->internal_failure = 1;
            return NULL;
        }
        left = node;
    }
    return left;
}

/*
 * Analiza expresiones de igualdad y desigualdad.
 * Procesa primero las comparaciones relacionales y construye nodos binarios
 * para los operadores == y !=.
 * equality_expression ::= comparison_expression
 *                         ( (EQUAL | NOT_EQUAL)
 *                           comparison_expression )*
 */

static ASTNode *parse_equality_expression(Parser *parser)
{
    ASTNode *left =
        parse_comparison_expression(parser);
    if (left == NULL)
        return NULL;
    while (parser_check(parser, EQUAL) ||
           parser_check(parser, NOT_EQUAL))
    {
        const Token *token = parser_current(parser);
        BinaryOperator operator =
            token->type == EQUAL
                ? OP_EQUAL
                : OP_NOT_EQUAL;
        int line = (int)token->line;
        int column = (int)token->column;
        if (!parser_advance(parser))
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *right =
            parse_comparison_expression(parser);
        if (right == NULL)
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *node = ast_create_binary(
            operator,
            left,
            right,
            line,
            column
        );
        if (node == NULL)
        {
            ast_destroy(left);
            ast_destroy(right);
            parser->internal_failure = 1;
            return NULL;
        }
        left = node;
    }
    return left;
}

/*
 * Analiza expresiones con operadores relacionales.
 * Procesa primero las expresiones aditivas y construye nodos binarios para
 * los operadores <, <=, > y >=.
 * comparison_expression ::= additive_expression
 *                           ( (LESS
 *                            | LESS_EQUAL
 *                            | GREATER
 *                            | GREATER_EQUAL)
 *                             additive_expression )*
 */

static ASTNode *parse_comparison_expression(Parser *parser)

{
    ASTNode *left =
        parse_additive_expression(parser);
    if (left == NULL)
        return NULL;
    while (parser_check(parser, LESS) ||
           parser_check(parser, LESS_EQUAL) ||
           parser_check(parser, GREATER) ||
           parser_check(parser, GREATER_EQUAL))
    {
        const Token *token = parser_current(parser);
        BinaryOperator operator;
        switch (token->type)
        {
            case LESS:
                operator = OP_LESS;
                break;
            case LESS_EQUAL:
                operator = OP_LESS_EQUAL;
                break;
            case GREATER:
                operator = OP_GREATER;
                break;
            case GREATER_EQUAL:
                operator = OP_GREATER_EQUAL;
                break;
            default:
                ast_destroy(left);
                return NULL;
        }
        int line = (int)token->line;
        int column = (int)token->column;
        if (!parser_advance(parser))
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *right =
            parse_additive_expression(parser);
        if (right == NULL)
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *node = ast_create_binary(
            operator,
            left,
            right,
            line,
            column
        );
        if (node == NULL)
        {
            ast_destroy(left);
            ast_destroy(right);
            parser->internal_failure = 1;
            return NULL;
        }
        left = node;
    }
    return left;
}

/*
 * Analiza expresiones de suma y resta.
 * Procesa primero las expresiones multiplicativas para respetar su mayor
 * precedencia y construye nodos binarios para + y -.
 * additive_expression ::= multiplicative_expression
 *                         ( (PLUS | MINUS)
 *                           multiplicative_expression )*
 */

static ASTNode *parse_additive_expression(Parser *parser)
{
    ASTNode *left =
        parse_multiplicative_expression(parser);
    if (left == NULL)
        return NULL;
    while (parser_check(parser, PLUS) ||
           parser_check(parser, MINUS))
    {
        const Token *token = parser_current(parser);
        BinaryOperator operator =
            token->type == PLUS
                ? OP_ADD
                : OP_SUBTRACT;
        int line = (int)token->line;
        int column = (int)token->column;
        if (!parser_advance(parser))
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *right =
            parse_multiplicative_expression(parser);
        if (right == NULL)
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *node = ast_create_binary(
            operator,
            left,
            right,
            line,
            column
        );
        if (node == NULL)
        {
            ast_destroy(left);
            ast_destroy(right);
            parser->internal_failure = 1;
            return NULL;
        }
        left = node;
    }
    return left;
}

/*
 * Analiza expresiones de multiplicación y división.
 * Procesa primero las expresiones unarias y construye nodos binarios para
 * los operadores * y /.
 * multiplicative_expression ::= unary_expression
 *                               ( (STAR | SLASH)
 *                                 unary_expression )*
 */

static ASTNode *parse_multiplicative_expression(Parser *parser)
{
    ASTNode *left = parse_unary_expression(parser);
    if (left == NULL)
        return NULL;
    while (parser_check(parser, STAR) ||
           parser_check(parser, SLASH))
    {
        const Token *token = parser_current(parser);
        BinaryOperator operator =
            token->type == STAR
                ? OP_MULTIPLY
                : OP_DIVIDE;
        int line = (int)token->line;
        int column = (int)token->column;
        if (!parser_advance(parser))
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *right = parse_unary_expression(parser);
        if (right == NULL)
        {
            ast_destroy(left);
            return NULL;
        }
        ASTNode *node = ast_create_binary(
            operator,
            left,
            right,
            line,
            column
        );
        if (node == NULL)
        {
            ast_destroy(left);
            ast_destroy(right);
            parser->internal_failure = 1;
            return NULL;
        }
        left = node;
    }
    return left;
}

/*
 * Analiza una expresión unaria.
 * Si encuentra '-', construye un nodo de negación cuyo operando puede ser
 * otra expresión unaria; en caso contrario delega el análisis a primary.
 * unary_expression ::= MINUS unary_expression
 *                    | primary
 */

static ASTNode *parse_unary_expression(Parser *parser)
{
    if (parser_check(parser, MINUS))
    {
        const Token *token = parser_current(parser);
        int line = token->line;
        int column = token->column;
        if (!parser_advance(parser))
            return NULL;
        ASTNode *operand = parse_unary_expression(parser);
        if (operand == NULL)
            return NULL;
        ASTNode *node = ast_create_unary(
            OP_NEGATE,
            operand,
            line,
            column
        );
        if (node == NULL)
        {
            ast_destroy(operand);
            parser->internal_failure = 1;
            return NULL;
        }
        return node;
    }
    return parse_primary(parser);

}

/*
 * Analiza los elementos primarios de una expresión.
 * Crea el nodo correspondiente para enteros, booleanos e identificadores.
 * Si encuentra '(', analiza una expresión completa y exige su ')' de cierre.
 * primary ::= INTEGER
 *           | TRUE
 *           | FALSE
 *           | IDENTIFIER
 *           | LPAREN expression RPAREN
 */

static ASTNode *parse_primary(Parser *parser)
{
    const Token *token = parser_current(parser);
    if (token == NULL)
        return NULL;
    if (parser_check(parser, INTEGER))
    {
        ASTNode *node = ast_create_integer(
            token->lexeme,
            token->line,
            token->column
        );
        if (node == NULL)
        {
            parser->internal_failure = 1;
            return NULL;
        }
        if (!parser_advance(parser))
        {
            ast_destroy(node);
            return NULL;
        }
        return node;
    }
    if (parser_check(parser, TRUE))
    {
        ASTNode *node = ast_create_boolean(
            1,
            token->line,
            token->column
        );
        if (node == NULL)
        {
            parser->internal_failure = 1;
            return NULL;
        }
        if (!parser_advance(parser))
        {
            ast_destroy(node);
            return NULL;
        }
        return node;
    }
    if (parser_check(parser, FALSE))
    {
        ASTNode *node = ast_create_boolean(
            0,
            token->line,
            token->column
        );
        if (node == NULL)
        {
            parser->internal_failure = 1;
            return NULL;
        }
        if (!parser_advance(parser))
        {
            ast_destroy(node);
            return NULL;
        }
        return node;
    }
    if (parser_check(parser, IDENTIFIER))
    {
        ASTNode *node = ast_create_identifier(
            token->lexeme,
            token->line,
            token->column
        );
        if (node == NULL)
        {
            parser->internal_failure = 1;
            return NULL;
        }
        if (!parser_advance(parser))
        {
            ast_destroy(node);
            return NULL;
        }
        return node;
    }
    if (parser_match(parser, LPAREN))
    {
        ASTNode *expression = parse_expression(parser);
        if (expression == NULL)
            return NULL;
        if (!parser_consume(
                parser,
                RPAREN,
                "')'"))
        {
            ast_destroy(expression);
            return NULL;
        }
        return expression;
    }
    syntax_error(
        parser,
        parser_current(parser),
        "una expresion"
    );
    synchronize(parser);
    return NULL;
}

/*
 * Analiza una declaración de variable de tipo int o bool.
 * Obtiene el identificador, procesa una expresión inicializadora opcional,
 * exige el ';' final y construye el nodo AST_VARIABLE_DECLARATION.
 * declaration ::= type IDENTIFIER
 *                 ( ASSIGN expression )?
 *                 SEMICOLON
 */

static ASTNode *parse_declaration(Parser *parser)
{
    ASTDeclaredType declared_type;
    if (parser_check(parser, INT))
        declared_type = AST_TYPE_INT;
    else
        declared_type = AST_TYPE_BOOL;
    if (!parser_advance(parser))
        return NULL;
    if (!parser_check(parser, IDENTIFIER))
    {
        syntax_error(
            parser,
            parser_current(parser),
            "un identificador"
        );
        synchronize(parser);
        return NULL;
    }
    const Token *token = parser_current(parser);
    char *name = copy_string(token->lexeme);
    if (name == NULL)
    {
        parser->internal_failure = 1;
        return NULL;
    }
    int line = (int)token->line;
    int column = (int)token->column;
    if (!parser_advance(parser))
    {
        free(name);
        return NULL;
    }
    ASTNode *initializer = NULL;
    if (parser_match(parser, ASSIGN))
    {
        initializer = parse_expression(parser);
        if (initializer == NULL)
        {
            free(name);
            return NULL;
        }
    }
    if (!parser_consume(
            parser,
            SEMICOLON,
            "';'"))
    {
        free(name);
        ast_destroy(initializer);
        return NULL;
    }
    ASTNode *node = ast_create_variable_declaration(
        declared_type,
        name,
        initializer,
        line,
        column
    );
    free(name);
    if (node == NULL)
    {
        ast_destroy(initializer);
        parser->internal_failure = 1;
        return NULL;
    }
    return node;
}

/*
 * Analiza una asignación a un identificador.
 * Obtiene el nombre de la variable, exige '=', procesa la expresión asignada,
 * exige el ';' final y construye el nodo AST_ASSIGNMENT.
 * assignment ::= IDENTIFIER ASSIGN expression SEMICOLON
 */

static ASTNode *parse_assignment(Parser *parser)
{
    const Token *token = parser_current(parser);
    if (token == NULL)
        return NULL;
    char *name = copy_string(token->lexeme);
    if (name == NULL)
    {
        parser->internal_failure = 1;
        return NULL;
    }
    int line = (int)token->line;
    int column = (int)token->column;
    if (!parser_advance(parser))
    {
        free(name);
        return NULL;
    }
    if (!parser_consume(
            parser,
            ASSIGN,
            "'='"))
    {
        free(name);
        return NULL;
    }
    ASTNode *value = parse_expression(parser);
    if (value == NULL)
    {
        free(name);
        return NULL;
    }
    if (!parser_consume(
            parser,
            SEMICOLON,
            "';'"))
    {
        free(name);
        ast_destroy(value);
        return NULL;
    }
    ASTNode *node = ast_create_assignment(
        name,
        value,
        line,
        column
    );
    free(name);
    if (node == NULL)
    {
        ast_destroy(value);
        parser->internal_failure = 1;
        return NULL;
    }
    return node;
}

/*
 * Analiza un bloque de sentencias delimitado por llaves.
 * Procesa y almacena cada sentencia hasta encontrar el cierre del bloque y
 * construye un nodo AST_BLOCK con la lista resultante.
 * block ::= LBRACE statement* RBRACE
 */

static ASTNode *parse_block(Parser *parser)
{
    const Token *token = parser_current(parser);
    if (token == NULL)
        return NULL;
    int line = (int)token->line;
    int column = (int)token->column;
    if (!parser_advance(parser))
        return NULL;
    ASTNodeList statements;
    ast_node_list_init(&statements);
    while (!parser_check(parser, RBRACE) &&
           !parser_check(parser, ELSE) &&
           !parser_check(parser, TOKEN_EOF))
    {
        ASTNode *statement = parse_statement(parser);
        if (parser->internal_failure)
        {
            ast_node_list_destroy(&statements);
            return NULL;
        }
        if (statement != NULL)
        {
            if (!ast_node_list_append(
                    &statements,
                    statement))
            {
                ast_destroy(statement);
                ast_node_list_destroy(&statements);
                parser->internal_failure = 1;
                return NULL;
            }
        }
    }
    if (!parser_consume(
            parser,
            RBRACE,
            "'}'"))
    {
        ast_node_list_destroy(&statements);
        return NULL;
    }
    ASTNode *node = ast_create_block(
        statements,
        line,
        column
    );
    if (node == NULL)
    {
        ast_node_list_destroy(&statements);
        parser->internal_failure = 1;
        return NULL;
    }
    return node;
}

/*
 * Analiza una sentencia while.
 * Procesa la condición entre paréntesis y después una sentencia como cuerpo,
 * que también puede ser un bloque, para construir un nodo AST_WHILE.
 * while_statement ::= WHILE LPAREN expression RPAREN statement
 */
static ASTNode *parse_while_statement(Parser *parser)
{
    const Token *token = parser_current(parser);
    if (token == NULL)
        return NULL;
    int line = (int)token->line;
    int column = (int)token->column;
    if (!parser_advance(parser))
        return NULL;
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return NULL;
    }

    ASTNode *condition = parse_expression(parser);
    if (condition == NULL)
        return NULL;
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        ast_destroy(condition);
        return NULL;
    }
    ASTNode *body = parse_statement(parser);
    if (body == NULL)
    {
        ast_destroy(condition);
        return NULL;
    }
    ASTNode *node = ast_create_while(
        condition,
        body,
        line,
        column
    );
    if (node == NULL)
    {
        ast_destroy(condition);
        ast_destroy(body);
        parser->internal_failure = 1;
        return NULL;
    }
    return node;
}

/*
 * Analiza una sentencia if con una rama else opcional.
 * Procesa la condición, la sentencia de la rama then y, si aparece ELSE,
 * la sentencia de la rama alternativa para construir un nodo AST_IF.
 * if_statement ::= IF LPAREN expression RPAREN statement
 *                  ( ELSE statement )?
 */
static ASTNode *parse_if_statement(Parser *parser)
{
    const Token *token = parser_current(parser);
    if (token == NULL)
        return NULL;
    int line = (int)token->line;
    int column = (int)token->column;
    if (!parser_advance(parser))
        return NULL;
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return NULL;
    }
    ASTNode *condition = parse_expression(parser);
    if (condition == NULL)
        return NULL;
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        ast_destroy(condition);
        return NULL;
    }
    ASTNode *then_branch = parse_statement(parser);
    if (then_branch == NULL)
    {
        ast_destroy(condition);
        return NULL;
    }
    ASTNode *else_branch = NULL;
    if (parser_match(parser, ELSE))
    {
        else_branch = parse_statement(parser);
        if (else_branch == NULL)
        {
            ast_destroy(condition);
            ast_destroy(then_branch);
            return NULL;
        }
    }
    ASTNode *node = ast_create_if(
        condition,
        then_branch,
        else_branch,
        line,
        column
    );
    if (node == NULL)
    {
        ast_destroy(condition);
        ast_destroy(then_branch);
        ast_destroy(else_branch);
        parser->internal_failure = 1;
        return NULL;
    }
    return node;
}

/*
 * Analiza una sentencia print.
 * Exige una expresión entre paréntesis y el ';' final, y construye un nodo
 * AST_PRINT. La expresión es obligatoria, por lo que print() es inválido.
 * print_statement ::= PRINT LPAREN expression RPAREN SEMICOLON
 */

static ASTNode *parse_print_statement(Parser *parser)
{
    const Token *token = parser_current(parser);
    if (token == NULL)
        return NULL;
    int line = (int)token->line;
    int column = (int)token->column;
    if (!parser_advance(parser))
        return NULL;
    if (!parser_consume(
            parser,
            LPAREN,
            "'('"))
    {
        return NULL;
    }
    ASTNode *expression = parse_expression(parser);
    if (expression == NULL)
        return NULL;
    if (!parser_consume(
            parser,
            RPAREN,
            "')'"))
    {
        ast_destroy(expression);
        return NULL;
    }
    if (!parser_consume(
            parser,
            SEMICOLON,
            "';'"))
    {
        ast_destroy(expression);
        return NULL;
    }
    ASTNode *node = ast_create_print(
        expression,
        line,
        column
    );
    if (node == NULL)
    {
        ast_destroy(expression);
        parser->internal_failure = 1;
        return NULL;
    }
    return node;
}

/*
 * Determina qué tipo de sentencia comienza en el lookahead actual y delega
 * su análisis a la función correspondiente.
 * statement ::= declaration
 *             | assignment
 *             | print_statement
 *             | if_statement
 *             | while_statement
 *             | block
 *
 * Si el token no puede iniciar una sentencia válida, reporta el error,
 * intenta sincronizar el parser y devuelve NULL.
 */

static ASTNode *parse_statement(Parser *parser)
{
    if (parser_check(parser, INT) ||
        parser_check(parser, BOOL))
    {
        return parse_declaration(parser);
    }
    if (parser_check(parser, IDENTIFIER))
    {
        return parse_assignment(parser);
    }
    if (parser_check(parser, PRINT))
    {
        return parse_print_statement(parser);
    }
    if (parser_check(parser, IF))
    {
        return parse_if_statement(parser);
    }
    if (parser_check(parser, WHILE))
    {
        return parse_while_statement(parser);
    }
    if (parser_check(parser, LBRACE))
    {
        return parse_block(parser);
    }
    syntax_error(
        parser,
        parser_current(parser),
        "una sentencia"
    );
    if (!parser_advance(parser))
        return NULL;
    synchronize(parser);
    return NULL;
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
 * Procesa sentencias hasta alcanzar TOKEN_EOF y las almacena en una lista.
 * Si no hubo errores léxicos, sintácticos ni fallos internos, construye y
 * devuelve el nodo AST_PROGRAM; en caso contrario devuelve NULL.
 * program ::= statement* TOKEN_EOF
 */
ASTNode *parser_parse(Parser *parser)
{
    if (parser == NULL ||
        !parser->has_current ||
        parser->internal_failure)
    {
        return NULL;
    }
    ASTNodeList statements;
    ast_node_list_init(&statements);
    while (!parser_check(parser, TOKEN_EOF))
    {
        ASTNode *statement = parse_statement(parser);
        if (parser->internal_failure)
        {
            ast_node_list_destroy(&statements);
            return NULL;
        }
        if (statement != NULL)
        {
            if (!ast_node_list_append(
                    &statements,
                    statement))
            {
                ast_destroy(statement);
                ast_node_list_destroy(&statements);
                parser->internal_failure = 1;
                return NULL;
            }
        }
        if (!parser->has_current)
        {
            ast_node_list_destroy(&statements);
            return NULL;
        }
    }
    /*
     * Si hubo cualquier error léxico o sintáctico,
     * no se entrega un AST parcial.
     */
    if (parser->had_error)
    {
        ast_node_list_destroy(&statements);
        return NULL;
    }
    int line;
    int column;
    /*
     * Program toma la posición de la primera
     * sentencia. Si está vacío, la posición
     * corresponde a TOKEN_EOF.
     */
    if (statements.count > 0)
    {
        line = statements.items[0]->line;
        column = statements.items[0]->column;
    }
    else
    {
        const Token *token = parser_current(parser);
        line = (int)token->line;
        column = (int)token->column;
    }
    ASTNode *program = ast_create_program(
        statements,
        line,
        column
    );
    if (program == NULL)
    {
        ast_node_list_destroy(&statements);
        parser->internal_failure = 1;
        return NULL;
    }
    return program;
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
