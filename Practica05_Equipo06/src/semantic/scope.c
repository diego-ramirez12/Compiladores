#include "semantic/scope.h"
#include <stdlib.h>

Scope *scope_create(Scope *parent)
{
    Scope *scope = malloc(sizeof(*scope));
    if (scope == NULL) {
        return NULL;
    }
    scope->symbols = NULL;
    scope->count = 0;
    scope->capacity = 0;
    scope->level = parent == NULL ? 0 : parent->level + 1;
    scope->parent = parent;
    scope->next_owned = NULL;
    return scope;
}

void scope_destroy(Scope *scope)
{
    if (scope == NULL) {
        return;
    }
    for (size_t i = 0; i < scope->count; i++) {
        symbol_destroy(scope->symbols[i]);
    }

    free(scope->symbols);
    free(scope);
}
