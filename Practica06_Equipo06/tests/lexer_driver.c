#include "lexer/lexer.h"
#include "lexer/token.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *file;
    Lexer lexer;
    Token token;
    LexerStatus status;

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <archivo.mc>\n", argv[0]);
        return 2;
    }

    file = fopen(argv[1], "r");

    if (file == NULL)
    {
        fprintf(stderr, "Error: no se pudo abrir el archivo.\n");
        return 2;
    }

    if (!lexer_init(&lexer, file))
    {
        fprintf(stderr, "Error: no se pudo inicializar el lexer.\n");
        fclose(file);
        return 2;
    }

    for (;;)
    {
        status = lexer_next_token(&lexer, &token);

        if (status == LEXER_STATUS_IO_ERROR)
        {
            fprintf(stderr, "Error: no se pudo leer el archivo.\n");
            lexer_destroy(&lexer);
            fclose(file);
            return 2;
        }

        if (status == LEXER_STATUS_MEMORY_ERROR)
        {
            fprintf(stderr, "Error: no se pudo reservar memoria.\n");
            lexer_destroy(&lexer);
            fclose(file);
            return 2;
        }

        token_print(&token);

        if (token.type == TOKEN_EOF)
        {
            token_destroy(&token);
            break;
        }

        token_destroy(&token);
    }

    lexer_destroy(&lexer);
    fclose(file);

    return 0;
}
