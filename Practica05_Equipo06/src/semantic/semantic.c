#include "semantic/semantic.h"
#include "ast/ast.h"
#include <stdio.h>

static void visit_program(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_statement(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_block(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_declaration(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_assignment(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_expression(SemanticAnalyzer *analyzer, ASTNode *node);

void semantic_report_internal_error(SemanticAnalyzer *analyzer,
                                    const char *message)
{
    if (analyzer == NULL) {
        return;
    }
    if (!analyzer->had_internal_error) {
        fprintf(
            stderr,
            "Error interno: %s\n",
            message != NULL ? message : "fallo semantico."
        );
    }
    analyzer->had_internal_error = 1;
}

static void report_undeclared(
    SemanticAnalyzer *analyzer,
    const char *name,
    int line,
    int column)
{
    fprintf(
        stderr,
        "Error semantico [%d:%d]: "
        "la variable '%s' no ha sido declarada.\n",
        line,
        column,
        name
    );
    analyzer->error_count++;
}

static void report_redeclaration(
    SemanticAnalyzer *analyzer,
    const char *name,
    int line,
    int column,
    const Symbol *previous)
{
    fprintf(
        stderr,
        "Error semantico [%d:%d]: "
        "la variable '%s' ya fue declarada en este ambito. "
        "Declaracion anterior [%d:%d].\n",
        line,
        column,
        name,
        previous->line,
        previous->column
    );
    analyzer->error_count++;
}

static int declared_type_to_type(
    ASTDeclaredType declared_type,
    Type *result)
{
    if (result == NULL) {
        return 0;
    }
    switch (declared_type) {
        case AST_TYPE_INT:
            *result = TYPE_INT;
            return 1;
        case AST_TYPE_BOOL:
            *result = TYPE_BOOL;
            return 1;
        default:
            return 0;
    }
}

SemanticStatus semantic_analyzer_init(SemanticAnalyzer *analyzer)
{
    if (analyzer == NULL) {
        return SEMANTIC_INTERNAL_ERROR;
    }
    analyzer->error_count = 0;
    analyzer->had_internal_error = 0;
    analyzer->symbol_table.current_scope = NULL;
    analyzer->symbol_table.owned_scopes = NULL;
    if (symbol_table_init(&analyzer->symbol_table)
        != SYMBOL_TABLE_OK) {
        semantic_report_internal_error(
            analyzer,
            "no fue posible inicializar la tabla de simbolos."
        );
        return SEMANTIC_INTERNAL_ERROR;
    }
    return SEMANTIC_OK;
}

static void visit_program(SemanticAnalyzer *analyzer, ASTNode *node)
{
    ASTNodeList *statements = &node->data.program.statements;
    for (size_t i = 0; i < statements->count; i++) {
        if (analyzer->had_internal_error) {
            break;
        }
        visit_statement(analyzer, statements->items[i]);
    }
}

static void visit_block(SemanticAnalyzer *analyzer, ASTNode *node)
{
    if (symbol_table_enter_scope(&analyzer->symbol_table)
        != SYMBOL_TABLE_OK) {
        semantic_report_internal_error(
            analyzer,
            "no fue posible crear un ambito."
        );
        return;
    }

    ASTNodeList *statements = &node->data.block.statements;
    for (size_t i = 0; i < statements->count; i++) {
        if (analyzer->had_internal_error) {
            break;
        }
        visit_statement(analyzer, statements->items[i]);
    }
    symbol_table_exit_scope(&analyzer->symbol_table);
}

static void visit_declaration(
    SemanticAnalyzer *analyzer,
    ASTNode *node)
{
    const char *name = node->data.variable_declaration.name;
    Type type;
    if (name == NULL ||
        !declared_type_to_type(
            node->data.variable_declaration.declared_type,
            &type)) {
        semantic_report_internal_error(
            analyzer,
            "declaracion invalida en el AST."
        );

        return;
    }
    Symbol *previous = symbol_table_lookup_current(
        &analyzer->symbol_table,
        name
    );
    if (previous != NULL) {
        report_redeclaration(
            analyzer,
            name,
            node->line,
            node->column,
            previous
        );
    } else {
        int level = analyzer->symbol_table.current_scope->level;
        Symbol *symbol = symbol_create(
            name,
            type,
            level,
            node->line,
            node->column,
            node
        );
        if (symbol == NULL) {
            semantic_report_internal_error(
                analyzer,
                "no fue posible crear un simbolo."
            );
            return;
        }
        SymbolTableStatus status = symbol_table_insert(
            &analyzer->symbol_table,
            symbol
        );
        if (status != SYMBOL_TABLE_OK) {
            symbol_destroy(symbol);
            semantic_report_internal_error(
                analyzer,
                "no fue posible insertar un simbolo."
            );
            return;
        }
    }

    if (node->data.variable_declaration.initializer != NULL) {
        visit_expression(
            analyzer,
            node->data.variable_declaration.initializer
        );
    }
}

static void visit_assignment(
    SemanticAnalyzer *analyzer,
    ASTNode *node)
{
    const char *name = node->data.assignment.name;
    if (name == NULL) {
        semantic_report_internal_error(
            analyzer,
            "asignacion invalida en el AST."
        );
        return;
    }
    if (symbol_table_lookup(
            &analyzer->symbol_table, name) == NULL) {
        report_undeclared(
            analyzer,
            name,
            node->line,
            node->column
        );
    }
    visit_expression(analyzer, node->data.assignment.value);
}

static void visit_expression(
    SemanticAnalyzer *analyzer,
    ASTNode *node)
{
    if (analyzer->had_internal_error) {
        return;
    }
    if (node == NULL) {
        semantic_report_internal_error(
            analyzer,
            "expresion incompleta en el AST."
        );
        return;
    }
    switch (node->type) {
        case AST_IDENTIFIER:
            if (node->data.identifier.name == NULL) {
                semantic_report_internal_error(
                    analyzer,
                    "identificador invalido en el AST."
                );
            } else if (symbol_table_lookup(
                           &analyzer->symbol_table,
                           node->data.identifier.name) == NULL) {

                report_undeclared(
                    analyzer,
                    node->data.identifier.name,
                    node->line,
                    node->column
                );
            }
            break;
        case AST_BINARY_EXPRESSION:
            visit_expression(
                analyzer,
                node->data.binary.left
            );
            if (!analyzer->had_internal_error) {
                visit_expression(
                    analyzer,
                    node->data.binary.right
                );
            }
            break;
        case AST_UNARY_EXPRESSION:
            visit_expression(
                analyzer,
                node->data.unary.operand
            );
            break;
        case AST_INTEGER_LITERAL:
        case AST_BOOLEAN_LITERAL:
            break;

        default:
            semantic_report_internal_error(
                analyzer,
                "nodo de expresion inesperado."
            );
            break;
    }
}

static void visit_statement(
    SemanticAnalyzer *analyzer,
    ASTNode *node)
{
    if (analyzer->had_internal_error) {
        return;
    }
    if (node == NULL) {
        semantic_report_internal_error(
            analyzer,
            "sentencia incompleta en el AST."
        );
        return;
    }
    switch (node->type) {
        case AST_VARIABLE_DECLARATION:
            visit_declaration(analyzer, node);
            break;
        case AST_ASSIGNMENT:
            visit_assignment(analyzer, node);
            break;
        case AST_PRINT:
            visit_expression(
                analyzer,
                node->data.print_statement.expression
            );
            break;
        case AST_BLOCK:
            visit_block(analyzer, node);
            break;
        case AST_IF:
            visit_expression(
                analyzer,
                node->data.if_statement.condition
            );
            if (!analyzer->had_internal_error) {
                visit_statement(
                    analyzer,
                    node->data.if_statement.then_branch
                );
            }
            if (!analyzer->had_internal_error &&
                node->data.if_statement.else_branch != NULL) {
                visit_statement(
                    analyzer,
                    node->data.if_statement.else_branch
                );
            }
            break;
        case AST_WHILE:
            visit_expression(
                analyzer,
                node->data.while_statement.condition
            );
            if (!analyzer->had_internal_error) {
                visit_statement(
                    analyzer,
                    node->data.while_statement.body
                );
            }
            break;
        default:
            semantic_report_internal_error(
                analyzer,
                "nodo de sentencia inesperado."
            );
            break;
    }
}

SemanticStatus semantic_analyze(
    SemanticAnalyzer *analyzer,
    ASTNode *program)
{
    if (analyzer == NULL) {
        return SEMANTIC_INTERNAL_ERROR;
    }
    if (analyzer->had_internal_error) {
        return SEMANTIC_INTERNAL_ERROR;
    }
    if (program == NULL ||
        program->type != AST_PROGRAM ||
        analyzer->symbol_table.current_scope == NULL) {
        semantic_report_internal_error(
            analyzer,
            "programa o tabla de simbolos invalidos."
        );
        return SEMANTIC_INTERNAL_ERROR;
    }
    visit_program(analyzer, program);
    if (analyzer->had_internal_error) {
        return SEMANTIC_INTERNAL_ERROR;
    }
    return analyzer->error_count == 0
        ? SEMANTIC_OK
        : SEMANTIC_INVALID;
}

void semantic_analyzer_destroy(SemanticAnalyzer *analyzer)
{
    if (analyzer == NULL) {
        return;
    }
    symbol_table_destroy(&analyzer->symbol_table);
    analyzer->error_count = 0;
    analyzer->had_internal_error = 0;
}
