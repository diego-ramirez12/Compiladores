#include "ast/ast.h"
#include "semantic/semantic.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{

    ASTNode global_x = {
        .type = AST_VARIABLE_DECLARATION,
        .line = 1,
        .column = 4,
        .data.variable_declaration = {
            .declared_type = AST_TYPE_INT,
            .name = "x"
        }
    };

    ASTNode global_use = {
        .type = AST_IDENTIFIER,
        .line = 2,
        .column = 4,
        .data.identifier = {.name = "x"}
    };

    ASTNode global_assignment = {
        .type = AST_ASSIGNMENT,
        .line = 2,
        .column = 0,
        .data.assignment = {
            .name = "x",
            .value = &global_use
        }
    };

    ASTNode local_x = {
        .type = AST_VARIABLE_DECLARATION,
        .line = 4,
        .column = 9,
        .data.variable_declaration = {
            .declared_type = AST_TYPE_BOOL,
            .name = "x"
        }
    };

    ASTNode boolean_value = {
        .type = AST_BOOLEAN_LITERAL,
        .line = 5,
        .column = 8,
        .data.boolean_literal = {.value = 1}
    };

    ASTNode local_assignment = {
        .type = AST_ASSIGNMENT,
        .line = 5,
        .column = 4,
        .data.assignment = {
            .name = "x",
            .value = &boolean_value
        }
    };

    ASTNode *block_statements[] = {
        &local_x,
        &local_assignment
    };

    ASTNode block = {
        .type = AST_BLOCK,
        .line = 3,
        .column = 0,
        .data.block = {
            .statements = {
                .items = block_statements,
                .count = 2,
                .capacity = 2
            }
        }
    };

    ASTNode *valid_statements[] = {
        &global_x,
        &global_assignment,
        &block
    };

    ASTNode valid_program = {
        .type = AST_PROGRAM,
        .line = 1,
        .column = 0,
        .data.program = {
            .statements = {
                .items = valid_statements,
                .count = 3,
                .capacity = 3
            }
        }
    };
    SemanticAnalyzer analyzer = {0};
    assert(
        semantic_analyzer_init(&analyzer)
        == SEMANTIC_OK
    );
    assert(
        semantic_analyze(&analyzer, &valid_program)
        == SEMANTIC_OK
    );

    assert(analyzer.error_count == 0);
    Symbol *resolved = symbol_table_lookup(
        &analyzer.symbol_table,
        "x"
    );

    assert(resolved != NULL);
    assert(resolved->type == TYPE_INT);
    assert(resolved->declaration == &global_x);

    semantic_analyzer_destroy(&analyzer);

    ASTNode undeclared_assignment = {
        .type = AST_ASSIGNMENT,
        .line = 1,
        .column = 0,
        .data.assignment = {
            .name = "y",
            .value = &boolean_value
        }
    };

    ASTNode duplicate_x = {
        .type = AST_VARIABLE_DECLARATION,
        .line = 3,
        .column = 5,
        .data.variable_declaration = {
            .declared_type = AST_TYPE_BOOL,
            .name = "x"
        }
    };

    ASTNode missing_identifier = {
        .type = AST_IDENTIFIER,
        .line = 4,
        .column = 6,
        .data.identifier = {.name = "z"}
    };

    ASTNode print_statement = {
        .type = AST_PRINT,
        .line = 4,
        .column = 0,
        .data.print_statement = {
            .expression = &missing_identifier
        }
    };

    ASTNode *invalid_statements[] = {
        &undeclared_assignment,
        &global_x,
        &duplicate_x,
        &print_statement
    };

    ASTNode invalid_program = {
        .type = AST_PROGRAM,
        .line = 1,
        .column = 0,
        .data.program = {
            .statements = {
                .items = invalid_statements,
                .count = 4,
                .capacity = 4
            }
        }
    };
    assert(
        semantic_analyzer_init(&analyzer)
        == SEMANTIC_OK
    );
    assert(
        semantic_analyze(&analyzer, &invalid_program)
        == SEMANTIC_INVALID
    );
    assert(analyzer.error_count == 3);
    resolved = symbol_table_lookup(
        &analyzer.symbol_table,
        "x"
    );
    assert(resolved != NULL);
    assert(resolved->type == TYPE_INT);
    assert(resolved->declaration == &global_x);
    semantic_analyzer_destroy(&analyzer);

    printf("[OK] SemanticAnalyzer\n");

    return 0;
}
