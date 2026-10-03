#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *file;
    Lexer lexer;
    Parser parser;
    ASTNode *root;

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

        lexer_destroy(&lexer);
        fclose(file);

        return 2;
    }

    /*
     * El parser devuelve la raíz del AST.
     * Si el programa es inválido, devuelve NULL.
     */
    root = parser_parse(&parser);

    if (parser.internal_failure)
    {
        fprintf(
            stderr,
            "Error interno durante el analisis.\n"
        );

        ast_destroy(root);
        parser_destroy(&parser);
        lexer_destroy(&lexer);
        fclose(file);

        return 2;
    }

    /*
     * El AST únicamente se imprime cuando
     * todo el programa es válido.
     */
    if (root != NULL)
    {
        printf("AST:\n");

        ast_print(root);

        printf(
            "Programa sintacticamente correcto.\n"
        );
    }

    ast_destroy(root);

    parser_destroy(&parser);
    lexer_destroy(&lexer);
    fclose(file);

    return root != NULL ? 0 : 1;
}