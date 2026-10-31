#include "semantic/semantic.h"
#include "ast/ast.h"
#include <stdio.h>

static void visit_program(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_statement(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_block(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_declaration(SemanticAnalyzer *analyzer, ASTNode *node);
static void visit_assignment(SemanticAnalyzer *analyzer, ASTNode *node);
static Type visit_expression(SemanticAnalyzer *analyzer, ASTNode *node);

static void visit_condition(
    SemanticAnalyzer *analyzer,
    ASTNode *condition,
    const char *statement_name
);


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
static const char *type_to_string(Type type)
{
    switch (type) {
        case TYPE_INT:
            return "int";
        case TYPE_BOOL:
            return "bool";
        case TYPE_ERROR:
            return "error";
        case TYPE_UNKNOWN:
            return "desconocido";
        default:
            return "desconocido";
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
            ASTNode *initializer =
            node->data.variable_declaration.initializer;
            Type initializer_type = visit_expression(
                analyzer,
                initializer
            );
        if (analyzer->had_internal_error ||
            initializer_type == TYPE_ERROR) {
                return;
            }
        if (initializer_type == TYPE_UNKNOWN) {
            semantic_report_internal_error(
                analyzer,
                "no fue posible determinar el tipo del inicializador."
            );
            return;
        }
        if (initializer_type != type) {
            const char *expected_type =
            type == TYPE_INT ? "int" : "bool";
            const char *actual_type =
            initializer_type == TYPE_INT ? "int" : "bool";
            fprintf(
                stderr,
                "Error semantico [%d:%d]: "
                "no es posible inicializar una variable "
                "de tipo '%s' con una expresion de tipo '%s'.\n",
                initializer->line,
                initializer->column,
                expected_type,
                actual_type
            );
            analyzer->error_count++;
        }
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
    Symbol *symbol = symbol_table_lookup(
        &analyzer->symbol_table,
        name
    );
    if (symbol == NULL) {
        report_undeclared(
            analyzer,
            name,
            node->line,
            node->column
        );
    }
    Type value_type = visit_expression(
        analyzer,
        node->data.assignment.value
    );
    if (analyzer->had_internal_error ||
        symbol == NULL ||
        value_type == TYPE_ERROR) {
        return;
    }
    if (value_type == TYPE_UNKNOWN) {
        semantic_report_internal_error(
            analyzer,
            "no fue posible determinar el tipo de la asignacion."
        );
        return;
    }
    if (value_type != symbol->type) {
        const char *expected_type =
            symbol->type == TYPE_INT ? "int" : "bool";
        const char *actual_type =
            value_type == TYPE_INT ? "int" : "bool";
        fprintf(
            stderr,
            "Error semantico [%d:%d]: "
            "no es posible asignar un valor de tipo '%s' "
            "a una variable de tipo '%s'.\n",
            node->line,
            node->column,
            actual_type,
            expected_type
        );
        analyzer->error_count++;
    }
}

static void visit_condition(
    SemanticAnalyzer *analyzer,
    ASTNode *condition,
    const char *statement_name)
{
    Type condition_type = visit_expression(
        analyzer,
        condition
    );
    if (analyzer->had_internal_error ||
        condition_type == TYPE_ERROR) {
        return;
    }
    if (condition_type == TYPE_UNKNOWN) {
        semantic_report_internal_error(
            analyzer,
            "no fue posible determinar el tipo de la condicion."
        );
        return;
    }
    if (condition_type != TYPE_BOOL) {
        const char *found_type =
            condition_type == TYPE_INT ? "int" : "desconocido";
        fprintf(
            stderr,
            "Error semantico [%d:%d]: "
            "la condicion de una sentencia '%s' "
            "debe ser de tipo 'bool', pero se encontro '%s'.\n",
            condition->line,
            condition->column,
            statement_name,
            found_type
        );
        analyzer->error_count++;
    }
}

static Type visit_expression(
    SemanticAnalyzer *analyzer,
    ASTNode *node)
{
    if (analyzer->had_internal_error) {
        return TYPE_ERROR;
    }
    if (node == NULL) {
        semantic_report_internal_error(
            analyzer,
            "expresion incompleta en el AST."
        );
        return TYPE_ERROR;
    }
    switch (node->type) {
        case AST_IDENTIFIER: {
            const char *name = node->data.identifier.name;
            if (name == NULL) {
                semantic_report_internal_error(
                    analyzer,
                    "identificador invalido en el AST."
                );
                return TYPE_ERROR;
            }
            Symbol *symbol = symbol_table_lookup(
                &analyzer->symbol_table,
                name
            );
            if (symbol == NULL) {
                report_undeclared(
                    analyzer,
                    name,
                    node->line,
                    node->column
                );
                return TYPE_ERROR;
            }
            return symbol->type;
        }
        case AST_INTEGER_LITERAL:
            return TYPE_INT;
        case AST_BOOLEAN_LITERAL:
            return TYPE_BOOL;
        case AST_BINARY_EXPRESSION: {
    Type left_type = visit_expression(
        analyzer,
        node->data.binary.left
    );
    Type right_type = TYPE_ERROR;
    if (!analyzer->had_internal_error) {
        right_type = visit_expression(
            analyzer,
            node->data.binary.right
        );
    }
    if (analyzer->had_internal_error ||
        left_type == TYPE_ERROR ||
        right_type == TYPE_ERROR) {
        return TYPE_ERROR;
    }
    if (left_type == TYPE_UNKNOWN ||
        right_type == TYPE_UNKNOWN) {
        return TYPE_UNKNOWN;
    }
    switch (node->data.binary.operator) {
        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE: {
            if (left_type != TYPE_INT ||
                right_type != TYPE_INT) {
                const char *operator_name;
                switch (node->data.binary.operator) {
                    case OP_ADD:
                        operator_name = "+";
                        break;
                    case OP_SUBTRACT:
                        operator_name = "-";
                        break;
                    case OP_MULTIPLY:
                        operator_name = "*";
                        break;
                    default:
                        operator_name = "/";
                        break;
                }
                fprintf(
                    stderr,
                    "Error semantico [%d:%d]: "
                    "el operador '%s' requiere operandos de tipo 'int', "
                    "pero se encontraron '%s' y '%s'.\n",
                    node->line,
                    node->column,
                    operator_name,
                    type_to_string(left_type),
                    type_to_string(right_type)
                );
                analyzer->error_count++;
                return TYPE_ERROR;
            }
            return TYPE_INT;
        }
        case OP_LESS:
        case OP_LESS_EQUAL:
        case OP_GREATER:
        case OP_GREATER_EQUAL: {
            if (left_type != TYPE_INT ||
                right_type != TYPE_INT) {
                const char *operator_name;
                switch (node->data.binary.operator) {
                    case OP_LESS:
                        operator_name = "<";
                        break;
                    case OP_LESS_EQUAL:
                        operator_name = "<=";
                        break;
                    case OP_GREATER:
                        operator_name = ">";
                        break;
                    default:
                        operator_name = ">=";
                        break;
                }
                fprintf(
                    stderr,
                    "Error semantico [%d:%d]: "
                    "el operador '%s' requiere operandos de tipo 'int', "
                    "pero se encontraron '%s' y '%s'.\n",
                    node->line,
                    node->column,
                    operator_name,
                    type_to_string(left_type),
                    type_to_string(right_type)
                );
                analyzer->error_count++;
                return TYPE_ERROR;
            }
            return TYPE_BOOL;
        }
        case OP_EQUAL:
        case OP_NOT_EQUAL: {
            if (left_type != right_type) {
                const char *operator_name =
                    node->data.binary.operator == OP_EQUAL
                        ? "=="
                        : "!=";
                fprintf(
                    stderr,
                    "Error semantico [%d:%d]: "
                    "el operador '%s' requiere operandos del mismo tipo, "
                    "pero se encontraron '%s' y '%s'.\n",
                    node->line,
                    node->column,
                    operator_name,
                    type_to_string(left_type),
                    type_to_string(right_type)
                );
                analyzer->error_count++;
                return TYPE_ERROR;
            }
            return TYPE_BOOL;
        }
        case OP_AND:
        case OP_OR: {
            if (left_type != TYPE_BOOL ||
                right_type != TYPE_BOOL) {
                const char *operator_name =
                    node->data.binary.operator == OP_AND
                        ? "&&"
                        : "||";
                fprintf(
                    stderr,
                    "Error semantico [%d:%d]: "
                    "el operador '%s' requiere operandos de tipo 'bool', "
                    "pero se encontraron '%s' y '%s'.\n",
                    node->line,
                    node->column,
                    operator_name,
                    type_to_string(left_type),
                    type_to_string(right_type)
                );
                analyzer->error_count++;
                return TYPE_ERROR;
            }
            return TYPE_BOOL;
        }
        default:
            return TYPE_UNKNOWN;
    }
        }
        case AST_UNARY_EXPRESSION: {
            Type operand_type = visit_expression(
                analyzer,
                node->data.unary.operand
            );
            if (analyzer->had_internal_error ||
                operand_type == TYPE_ERROR) {
                return TYPE_ERROR;
            }
            if (operand_type == TYPE_UNKNOWN) {
    return TYPE_UNKNOWN;
}
    switch (node->data.unary.operator) {
        case OP_NEGATE:
        if (operand_type != TYPE_INT) {
            fprintf(
                stderr,
                "Error semantico [%d:%d]: "
                "el operador unario '-' requiere un operando de tipo 'int', "
                "pero se encontro '%s'.\n",
                node->line,
                node->column,
                type_to_string(operand_type)
            );
            analyzer->error_count++;
            return TYPE_ERROR;
        }
        return TYPE_INT;
    default:
        semantic_report_internal_error(
            analyzer,
            "operador unario desconocido."
        );
        return TYPE_ERROR;
}
        }
        default:
            semantic_report_internal_error(
                analyzer,
                "nodo de expresion inesperado."
            );
            return TYPE_ERROR;
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
        visit_condition(
            analyzer,
            node->data.if_statement.condition,
            "if"
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
            visit_condition(
                analyzer,
                node->data.while_statement.condition,
                "while"
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
