#include "semantic/symbol_table.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

SymbolTableStatus symbol_table_init(SymbolTable *table)
{
    if (table == NULL) {
        return SYMBOL_TABLE_INTERNAL_ERROR;
    }
    table->current_scope = NULL;
    table->owned_scopes = NULL;
    Scope *global = scope_create(NULL);
    if (global == NULL) {
        return SYMBOL_TABLE_INTERNAL_ERROR;
    }
    table->current_scope = global;
    table->owned_scopes = global;

    return SYMBOL_TABLE_OK;
}


SymbolTableStatus symbol_table_enter_scope(SymbolTable *table)
{
    if (table == NULL || table->current_scope == NULL) {
        return SYMBOL_TABLE_INTERNAL_ERROR;
    }
    Scope *child = scope_create(table->current_scope);
    if (child == NULL) {
        return SYMBOL_TABLE_INTERNAL_ERROR;
    }
    child->next_owned = table->owned_scopes;
    table->owned_scopes = child;
    table->current_scope = child;

    return SYMBOL_TABLE_OK;
}

void symbol_table_exit_scope(SymbolTable *table)
{
    if (table == NULL || table->current_scope == NULL) {
        return;
    }
    if (table->current_scope->parent != NULL) {
        table->current_scope = table->current_scope->parent;
    }
}

Symbol *symbol_table_lookup_current(
    const SymbolTable *table,
    const char *name)
{
    if (table == NULL ||
        table->current_scope == NULL ||
        name == NULL) {
        return NULL;
    }

    const Scope *scope = table->current_scope;
    for (size_t i = 0; i < scope->count; i++) {
        Symbol *symbol = scope->symbols[i];

        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
    }

    return NULL;
}

Symbol *symbol_table_lookup(
    const SymbolTable *table,
    const char *name)
{
    if (table == NULL || name == NULL) {
        return NULL;
    }
    for (const Scope *scope = table->current_scope;
         scope != NULL;
         scope = scope->parent) {
        for (size_t i = 0; i < scope->count; i++) {
            Symbol *symbol = scope->symbols[i];
            if (strcmp(symbol->name, name) == 0) {
                return symbol;
            }
        }
    }
    return NULL;
}

SymbolTableStatus symbol_table_insert(
    SymbolTable *table,
    Symbol *symbol)
{
    if (table == NULL ||
        table->current_scope == NULL ||
        symbol == NULL ||
        symbol->name == NULL) {
        return SYMBOL_TABLE_INTERNAL_ERROR;
    }
    if (symbol_table_lookup_current(
            table, symbol->name) != NULL) {
        return SYMBOL_TABLE_DUPLICATE;
    }

    Scope *scope = table->current_scope;

    if (scope->count == scope->capacity) {
        size_t new_capacity;

        if (scope->capacity == 0) {
            new_capacity = 4;
        } else {
            if (scope->capacity > SIZE_MAX / 2) {
                return SYMBOL_TABLE_INTERNAL_ERROR;
            }
            new_capacity = scope->capacity * 2;
        }
        if (new_capacity >
            SIZE_MAX / sizeof(*scope->symbols)) {
            return SYMBOL_TABLE_INTERNAL_ERROR;
        }
        Symbol **new_symbols = realloc(
            scope->symbols,
            new_capacity * sizeof(*scope->symbols)
        );
        if (new_symbols == NULL) {
            return SYMBOL_TABLE_INTERNAL_ERROR;
        }
        scope->symbols = new_symbols;
        scope->capacity = new_capacity;
    }

    scope->symbols[scope->count++] = symbol;

    return SYMBOL_TABLE_OK;
}

void symbol_table_destroy(SymbolTable *table)
{
    if (table == NULL) {
        return;
    }

    Scope *scope = table->owned_scopes;
    while (scope != NULL) {
        Scope *next = scope->next_owned;
        scope_destroy(scope);
        scope = next;
    }

    table->current_scope = NULL;
    table->owned_scopes = NULL;
}
