#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast.h"
#include "semantic/semantic.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *file;
    Lexer lexer;
    Parser parser;
    ASTNode *root;
    SemanticAnalyzer analyzer = {0};
    SemanticStatus semantic_status;
    int exit_code;

    if (argc != 2)
    {
        fprintf(stderr, "Uso: ./minic <archivo.mc>\n");
        return 2;
    }

    file = fopen(argv[1], "r");

    if (file == NULL)
    {
        fprintf(
            stderr,
            "Error: no se pudo abrir el archivo '%s'.\n",
            argv[1]
        );

        return 2;
    }

    if (!lexer_init(&lexer, file))
    {
        fprintf(
            stderr,
            "Error: no se pudo inicializar el lexer.\n"
        );
        fclose(file);
        return 2;
    }

    if (!parser_init(&parser, &lexer))
    {
        fprintf(
            stderr,
            "Error: no se pudo inicializar el parser.\n"
        );
        parser_destroy(&parser);
        lexer_destroy(&lexer);
        fclose(file);

        return 2;
    }

    root = parser_parse(&parser);

    if (parser.internal_failure)
    {
        fprintf(
            stderr,
            "Error interno durante el analisis sintactico.\n"
        );

        ast_destroy(root);
        parser_destroy(&parser);
        lexer_destroy(&lexer);
        fclose(file);

        return 2;
    }

    if (root == NULL)
    {
        parser_destroy(&parser);
        lexer_destroy(&lexer);
        fclose(file);

        return 1;
    }

    semantic_status = semantic_analyzer_init(&analyzer);
    if (semantic_status != SEMANTIC_OK)
    {
        semantic_analyzer_destroy(&analyzer);
        ast_destroy(root);
        parser_destroy(&parser);
        lexer_destroy(&lexer);
        fclose(file);

        return 2;
    }

    semantic_status = semantic_analyze(&analyzer, root);
    if (semantic_status == SEMANTIC_OK)
    {
        printf("Programa semanticamente correcto.\n");
        exit_code = 0;
    }
    else if (semantic_status == SEMANTIC_INVALID)
    {
        exit_code = 1;
    }
    else
    {
        exit_code = 2;
    }

    semantic_analyzer_destroy(&analyzer);
    ast_destroy(root);
    parser_destroy(&parser);
    lexer_destroy(&lexer);
    fclose(file);

    return exit_code;
}
