#include "ast/ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static char *copy_string(const char *text) {

    if (text == NULL) {
        return NULL;
    }
    size_t length = strlen(text) + 1;
    char *copy = malloc(length);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, text, length);
    return copy;
}
/*
    Imprime la indentación utilizada al mostrar el árbol sintáctico.
    Por cada nivel de profundidad imprime dos espacios.
*/
static void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}
/*
    Convierte un operador binario del AST a su nombre textual.
    Devuelve la cadena correspondiente al tipo de operador recibido
    o "Unknown" si el operador no coincide con ninguno conocido.
*/
static const char *binary_operator_name(BinaryOperator operator) {
    switch (operator) {
        case OP_ADD:
            return "Add";
        case OP_SUBTRACT:
            return "Subtract";
        case OP_MULTIPLY:
            return "Multiply";
        case OP_DIVIDE:
            return "Divide";
        case OP_LESS:
            return "Less";
        case OP_LESS_EQUAL:
            return "LessEqual";
        case OP_GREATER:
            return "Greater";
        case OP_GREATER_EQUAL:
            return "GreaterEqual";
        case OP_EQUAL:
            return "Equal";
        case OP_NOT_EQUAL:
            return "NotEqual";
        case OP_AND:
            return "And";
        case OP_OR:
            return "Or";
    }
    return "Unknown";
}

/*
    Imprime recursivamente un nodo del árbol sintáctico abstracto
    respetando el nivel de indentación indicado. Según el tipo de nodo,
    muestra su información y continúa con sus nodos hijos.
*/
static void ast_print_indented(
    const ASTNode *node,
    int depth
) {
    if (node == NULL) {
        return;
    }
    print_indent(depth);
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            for (
                size_t i = 0;
                i < node->data.program.statements.count;
                i++
            ) {
                ast_print_indented(
                    node->data.program.statements.items[i],
                    depth + 1
                );
            }
            break;
        case AST_BLOCK:
            printf("Block\n");
            for (
                size_t i = 0;
                i < node->data.block.statements.count;
                i++
            ) {
                ast_print_indented(
                    node->data.block.statements.items[i],
                    depth + 1
                );
            }
            break;
        case AST_VARIABLE_DECLARATION:
            printf(
                "VariableDeclaration(%s, %s)\n",
                node->data.variable_declaration.declared_type == AST_TYPE_INT
                    ? "int"
                    : "bool",
                node->data.variable_declaration.name
            );
            ast_print_indented(
                node->data.variable_declaration.initializer,
                depth + 1
            );
            break;
        case AST_ASSIGNMENT:
            printf(
                "Assignment(%s)\n",
                node->data.assignment.name
            );
            ast_print_indented(
                node->data.assignment.value,
                depth + 1
            );
            break;
        case AST_PRINT:
            printf("Print\n");
            ast_print_indented(
                node->data.print_statement.expression,
                depth + 1
            );
            break;
        case AST_IF:
            printf("If\n");
            print_indent(depth + 1);
            printf("Condition\n");
            ast_print_indented(
                node->data.if_statement.condition,
                depth + 2
            );
            print_indent(depth + 1);
            printf("Then\n");
            ast_print_indented(
                node->data.if_statement.then_branch,
                depth + 2
            );
            if (node->data.if_statement.else_branch != NULL) {
                print_indent(depth + 1);
                printf("Else\n");
                ast_print_indented(
                    node->data.if_statement.else_branch,
                    depth + 2
                );
            }
            break;
        case AST_WHILE:
            printf("While\n");
            print_indent(depth + 1);
            printf("Condition\n");
            ast_print_indented(
                node->data.while_statement.condition,
                depth + 2
            );
            print_indent(depth + 1);
            printf("Body\n");
            ast_print_indented(
                node->data.while_statement.body,
                depth + 2
            );
            break;
        case AST_BINARY_EXPRESSION:
            printf(
                "%s\n",
                binary_operator_name(
                    node->data.binary.operator
                )
            );
            ast_print_indented(
                node->data.binary.left,
                depth + 1
            );
            ast_print_indented(
                node->data.binary.right,
                depth + 1
            );
            break;
        case AST_UNARY_EXPRESSION:
            printf("Negate\n");
            ast_print_indented(
                node->data.unary.operand,
                depth + 1
            );
            break;
        case AST_IDENTIFIER:
            printf(
                "Identifier(%s)\n",
                node->data.identifier.name
            );
            break;
        case AST_INTEGER_LITERAL:
            printf(
                "Integer(%s)\n",
                node->data.integer_literal.lexeme
            );
            break;
        case AST_BOOLEAN_LITERAL:
            printf(
                "Boolean(%s)\n",
                node->data.boolean_literal.value
                    ? "true"
                    : "false"
            );
            break;
    }
}

/*
    Inicializa una lista de nodos del AST vacía.
    Establece el arreglo de elementos en NULL y deja su cantidad
    y capacidad iniciales en cero.
*/
void ast_node_list_init(ASTNodeList *list) {
    if (list == NULL) {
        return;
    }
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}
/*
    Agrega un nodo al final de una lista de nodos del AST.
    Si la lista no tiene espacio disponible, aumenta dinámicamente
    su capacidad. Devuelve 1 si se agregó correctamente y 0 si ocurre
    un error.
*/
int ast_node_list_append(
    ASTNodeList *list,
    ASTNode *node
) {
    if (list == NULL || node == NULL) {
        return 0;
    }
    if (list->count == list->capacity) {
        size_t new_capacity;
        if (list->capacity == 0) {
            new_capacity = 4;
        } else {
            new_capacity = list->capacity * 2;
        }
        ASTNode **new_items = realloc(
            list->items,
            new_capacity * sizeof(ASTNode *)
        );
        if (new_items == NULL) {
            return 0;
        }
        list->items = new_items;
        list->capacity = new_capacity;
    }
    list->items[list->count] = node;
    list->count++;
    return 1;
}
/*
    Destruye una lista de nodos del AST.
    Libera recursivamente cada nodo almacenado, libera el arreglo
    interno y restablece los datos de la lista.
*/
void ast_node_list_destroy(ASTNodeList *list) {
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->count; i++) {
        ast_destroy(list->items[i]);
    }
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}
/*
    Crea un nodo de tipo programa.
    Guarda la lista de sentencias que forman el programa junto con
    la línea y columna donde comienza. Devuelve el nodo creado o NULL
    si no se pudo reservar memoria.
*/
ASTNode *ast_create_program(
    ASTNodeList statements,
    int line,
    int column
) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = AST_PROGRAM;
    node->line = line;
    node->column = column;
    node->data.program.statements = statements;
    return node;
}
/*
    Crea un nodo de tipo bloque.
    Almacena la lista de sentencias contenidas en el bloque y su
    posición dentro del código fuente. Devuelve el nodo creado o NULL
    si falla la reserva de memoria.
*/
ASTNode *ast_create_block(
    ASTNodeList statements,
    int line,
    int column
) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = AST_BLOCK;
    node->line = line;
    node->column = column;
    node->data.block.statements = statements;
    return node;
}
/*
    Crea un nodo que representa la declaración de una variable.
    Guarda el tipo declarado, una copia del nombre, su inicializador
    opcional y la posición donde aparece en el código fuente.
*/
ASTNode *ast_create_variable_declaration(
    ASTDeclaredType declared_type,
    const char *name,
    ASTNode *initializer,
    int line,
    int column
) {
    if (name == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    char *name_copy = copy_string(name);
    if (name_copy == NULL) {
        free(node);
        return NULL;
    }
    node->type = AST_VARIABLE_DECLARATION;
    node->line = line;
    node->column = column;
    node->data.variable_declaration.declared_type =
        declared_type;
    node->data.variable_declaration.name =
        name_copy;
    node->data.variable_declaration.initializer =
        initializer;
    return node;
}
/*
    Crea un nodo que representa una asignación.
    Guarda una copia del nombre de la variable, la expresión asignada
    y la posición de la asignación dentro del código fuente.
*/
ASTNode *ast_create_assignment(
    const char *name,
    ASTNode *value,
    int line,
    int column
) {
    if (name == NULL || value == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    char *name_copy = copy_string(name);
    if (name_copy == NULL) {
        free(node);
        return NULL;
    }
    node->type = AST_ASSIGNMENT;
    node->line = line;
    node->column = column;
    node->data.assignment.name =
        name_copy;
    node->data.assignment.value =
        value;
    return node;
}

/*
    Crea un nodo que representa una sentencia print.
    Almacena la expresión que debe imprimirse y la posición de la
    sentencia dentro del código fuente.
*/
ASTNode *ast_create_print(
    ASTNode *expression,
    int line,
    int column
) {
    if (expression == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = AST_PRINT;
    node->line = line;
    node->column = column;
    node->data.print_statement.expression =
        expression;
    return node;
}
/*
    Crea un nodo que representa una sentencia if.
    Almacena la condición, la rama then, una rama else opcional y
    la posición donde aparece la sentencia en el código fuente.
*/
ASTNode *ast_create_if(
    ASTNode *condition,
    ASTNode *then_branch,
    ASTNode *else_branch,
    int line,
    int column
) {
    if (condition == NULL || then_branch == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = AST_IF;
    node->line = line;
    node->column = column;
    node->data.if_statement.condition =
        condition;
    node->data.if_statement.then_branch =
        then_branch;
    node->data.if_statement.else_branch =
        else_branch;
    return node;
}

/*
    Crea un nodo que representa una sentencia while.
    Guarda la condición del ciclo, su cuerpo y la posición donde
    aparece dentro del código fuente.
*/
ASTNode *ast_create_while(
    ASTNode *condition,
    ASTNode *body,
    int line,
    int column
) {
    if (condition == NULL || body == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = AST_WHILE;
    node->line = line;
    node->column = column;
    node->data.while_statement.condition =
        condition;
    node->data.while_statement.body =
        body;
    return node;
}
/*
    Crea un nodo que representa una expresión binaria.
    Almacena el operador, el operando izquierdo, el operando derecho
    y la posición de la expresión dentro del código fuente.
*/
ASTNode *ast_create_binary(
    BinaryOperator operator,
    ASTNode *left,
    ASTNode *right,
    int line,
    int column
) {
    if (left == NULL || right == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = AST_BINARY_EXPRESSION;
    node->line = line;
    node->column = column;
    node->data.binary.operator =
        operator;
    node->data.binary.left =
        left;
    node->data.binary.right =
        right;
    return node;
}
/*
    Crea un nodo que representa una expresión unaria.
    Guarda el operador, su único operando y la posición de la
    expresión dentro del código fuente.
*/
ASTNode *ast_create_unary(
    UnaryOperator operator,
    ASTNode *operand,
    int line,
    int column
) {
    if (operand == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = AST_UNARY_EXPRESSION;
    node->line = line;
    node->column = column;
    node->data.unary.operator =
        operator;
    node->data.unary.operand =
        operand;
    return node;
}
/*
    Crea un nodo que representa un identificador.
    Guarda una copia del nombre del identificador y la posición donde
    aparece dentro del código fuente.
*/
ASTNode *ast_create_identifier(
    const char *name,
    int line,
    int column
) {
    if (name == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    char *name_copy = copy_string(name);
    if (name_copy == NULL) {
        free(node);
        return NULL;
    }
    node->type = AST_IDENTIFIER;
    node->line = line;
    node->column = column;
    node->data.identifier.name =
        name_copy;
    return node;
}
/*
    Crea un nodo que representa un literal entero.
    Guarda una copia del lexema original y la posición donde aparece
    dentro del código fuente.
*/
ASTNode *ast_create_integer(
    const char *lexeme,
    int line,
    int column
) {
    if (lexeme == NULL) {
        return NULL;
    }
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    char *lexeme_copy = copy_string(lexeme);
    if (lexeme_copy == NULL) {
        free(node);
        return NULL;
    }
    node->type = AST_INTEGER_LITERAL;
    node->line = line;
    node->column = column;
    node->data.integer_literal.lexeme =
        lexeme_copy;
    return node;
}

/*
    Crea un nodo que representa un literal booleano.
    Guarda el valor normalizado como verdadero o falso y la posición
    donde aparece dentro del código fuente.
*/
ASTNode *ast_create_boolean(
    int value,
    int line,
    int column
) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = AST_BOOLEAN_LITERAL;
    node->line = line;
    node->column = column;
    node->data.boolean_literal.value =
        value != 0;
    return node;
}
/*
    Imprime el árbol sintáctico abstracto completo.
    Inicia la impresión recursiva desde el nodo recibido con un nivel
    de indentación inicial de cero.
*/
void ast_print(const ASTNode *node) {
    ast_print_indented(node, 0);
}
/*
    Libera recursivamente la memoria ocupada por un nodo del AST.
    Según el tipo de nodo, primero libera sus datos asociados y nodos
    hijos, y al final libera el propio nodo.
*/
void ast_destroy(ASTNode *node) {
    if (node == NULL) {
        return;
    }
    switch (node->type) {
        case AST_PROGRAM:
            ast_node_list_destroy(
                &node->data.program.statements
            );
            break;
        case AST_BLOCK:
            ast_node_list_destroy(
                &node->data.block.statements
            );
            break;
        case AST_VARIABLE_DECLARATION:
            free(
                node->data.variable_declaration.name
            );
            ast_destroy(
                node->data.variable_declaration.initializer
            );
            break;
        case AST_ASSIGNMENT:
            free(
                node->data.assignment.name
            );
            ast_destroy(
                node->data.assignment.value
            );
            break;
        case AST_PRINT:
            ast_destroy(
                node->data.print_statement.expression
            );
            break;
        case AST_IF:
            ast_destroy(
                node->data.if_statement.condition
            );
            ast_destroy(
                node->data.if_statement.then_branch
            );
            ast_destroy(
                node->data.if_statement.else_branch
            );
            break;
        case AST_WHILE:
            ast_destroy(
                node->data.while_statement.condition
            );
            ast_destroy(
                node->data.while_statement.body
            );
            break;
        case AST_BINARY_EXPRESSION:
            ast_destroy(
                node->data.binary.left
            );
            ast_destroy(
                node->data.binary.right
            );
            break;
        case AST_UNARY_EXPRESSION:
            ast_destroy(
                node->data.unary.operand
            );
            break;
        case AST_IDENTIFIER:
            free(
                node->data.identifier.name
            );
            break;
        case AST_INTEGER_LITERAL:
            free(
                node->data.integer_literal.lexeme
            );
            break;
        case AST_BOOLEAN_LITERAL:
            break;
    }
    free(node);
}