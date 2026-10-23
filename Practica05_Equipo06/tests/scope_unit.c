#include "semantic/scope.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    Scope *global = scope_create(NULL);

    assert(global != NULL);
    assert(global->level == 0);
    assert(global->parent == NULL);
    assert(global->symbols == NULL);
    assert(global->count == 0);
    assert(global->capacity == 0);
    assert(global->next_owned == NULL);

    global->symbols = malloc(sizeof(*global->symbols));
    assert(global->symbols != NULL);

    global->symbols[0] = symbol_create(
        "x", TYPE_INT, 0, 1, 4, NULL
    );

    assert(global->symbols[0] != NULL);

    global->count = 1;
    global->capacity = 1;

    Scope *block = scope_create(global);

    assert(block != NULL);
    assert(block->level == 1);
    assert(block->parent == global);

    block->symbols = malloc(sizeof(*block->symbols));
    assert(block->symbols != NULL);

    block->symbols[0] = symbol_create(
        "x", TYPE_BOOL, 1, 3, 8, NULL
    );

    assert(block->symbols[0] != NULL);
    block->count = 1;
    block->capacity = 1;

    Scope *nested = scope_create(block);
    assert(nested != NULL);
    assert(nested->level == 2);
    assert(nested->parent == block);

    scope_destroy(nested);
    scope_destroy(block);
    scope_destroy(global);
    scope_destroy(NULL);

    printf("[OK] Scope\n");

    return 0;
}
