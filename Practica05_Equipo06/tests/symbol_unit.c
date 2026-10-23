#include "semantic/symbol.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    char name[] = "contador";

    Symbol *symbol = symbol_create(
        name,
        TYPE_INT,
        0,
        1,
        4,
        NULL
    );

    assert(symbol != NULL);
    
    name[0] = 'X';
    assert(strcmp(symbol->name, "contador") == 0);

    assert(symbol->type == TYPE_INT);
    assert(symbol->scope_level == 0);
    assert(symbol->line == 1);
    assert(symbol->column == 4);
    assert(symbol->declaration == NULL);

    symbol_destroy(symbol);
    symbol_destroy(NULL);

    printf("[OK] Symbol\n");

    return 0;
}
