#include "lexer/lexer.h"
#include "parser/parser.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *file;
    Lexer lexer;
    Parser parser;
    int valid;

    /*
     * El programa debe recibir exactamente
     * un archivo fuente.
     */
    if (argc != 2)
    {
        fprintf(stderr, "Uso: ./minic <archivo.mc>\n");
        return 2;
    }

    /*
     * Abrimos el archivo fuente.
     */
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

    /*
     * Inicializamos el lexer incremental.
     */
    if (!lexer_init(&lexer, file))
    {
        fprintf(
            stderr,
            "Error: no se pudo inicializar el lexer.\n"
        );

        fclose(file);
        return 2;
    }

    /*
     * Inicializamos el parser.
     *
     * parser_init solicita también el primer
     * token de anticipación.
     */
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
     * Analizamos el programa completo.
     */
    valid = parser_parse(&parser);

    /*
     * Un fallo interno no corresponde a un
     * error del programa MiniC.
     */
    if (parser.internal_failure)
    {
        fprintf(
            stderr,
            "Error interno durante el analisis.\n"
        );

        parser_destroy(&parser);
        lexer_destroy(&lexer);
        fclose(file);

        return 2;
    }

    /*
     * stdout se reserva exclusivamente para
     * el resultado general del analisis.
     */
    if (valid)
    {
        printf(
            "Programa sintacticamente correcto.\n"
        );
    }

    /*
     * Liberamos recursos.
     *
     * parser_destroy libera su token actual.
     * lexer_destroy limpia el estado del lexer.
     * main conserva la propiedad del FILE *.
     */
    parser_destroy(&parser);
    lexer_destroy(&lexer);
    fclose(file);

    /*
     * 0: programa valido
     * 1: error lexico o sintactico
     */
    return valid ? 0 : 1;
}