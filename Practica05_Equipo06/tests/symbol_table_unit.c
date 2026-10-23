#include "semantic/symbol_table.h"
#include <assert.h>
#include <stdio.h>

static Symbol *new_symbol(
    const char *name,
    Type type,
    int level)
{
    Symbol *symbol = symbol_create(
        name, type, level, 1, 0, NULL
    );
    assert(symbol != NULL);

    return symbol;
}

int main(void)
{
    SymbolTable table = {0};
    assert(
        symbol_table_init(&table) == SYMBOL_TABLE_OK
    );
    assert(table.current_scope != NULL);
    assert(table.current_scope->level == 0);
    Symbol *global_x = new_symbol("x", TYPE_INT, 0);
    assert(
        symbol_table_insert(&table, global_x)
        == SYMBOL_TABLE_OK
    );
    assert(
        symbol_table_lookup_current(&table, "x")
        == global_x
    );
    Symbol *duplicate = new_symbol("x", TYPE_BOOL, 0);
    assert(
        symbol_table_insert(&table, duplicate)
        == SYMBOL_TABLE_DUPLICATE
    );
    assert(
        symbol_table_lookup(&table, "x")
        == global_x
    );
    symbol_destroy(duplicate);
    assert(
        symbol_table_enter_scope(&table)
        == SYMBOL_TABLE_OK
    );
    assert(table.current_scope->level == 1);
    assert(
        symbol_table_lookup_current(&table, "x")
        == NULL
    );
    assert(
        symbol_table_lookup(&table, "x")
        == global_x
    );
    Symbol *local_x = new_symbol("x", TYPE_BOOL, 1);
    assert(
        symbol_table_insert(&table, local_x)
        == SYMBOL_TABLE_OK
    );
    assert(
        symbol_table_lookup(&table, "x")
        == local_x
    );
    Symbol *local_y = new_symbol("y", TYPE_INT, 1);
    assert(
        symbol_table_insert(&table, local_y)
        == SYMBOL_TABLE_OK
    );
    assert(
        symbol_table_enter_scope(&table)
        == SYMBOL_TABLE_OK
    );
    assert(table.current_scope->level == 2);
    assert(
        symbol_table_lookup(&table, "x")
        == local_x
    );
    assert(
        symbol_table_lookup(&table, "y")
        == local_y
    );

    symbol_table_exit_scope(&table);
    assert(
        symbol_table_lookup(&table, "x")
        == local_x
    );
    symbol_table_exit_scope(&table);
    assert(
        symbol_table_lookup(&table, "x")
        == global_x
    );
    assert(
        symbol_table_lookup(&table, "y")
        == NULL
    );
    symbol_table_exit_scope(&table);

    assert(table.current_scope->level == 0);
    assert(
        symbol_table_enter_scope(&table)
        == SYMBOL_TABLE_OK
    );
    assert(
        symbol_table_lookup(&table, "y")
        == NULL
    );
    Symbol *sibling_y = new_symbol("y", TYPE_BOOL, 1);
    assert(
        symbol_table_insert(&table, sibling_y)
        == SYMBOL_TABLE_OK
    );
    assert(
        symbol_table_lookup(&table, "y")
        == sibling_y
    );
    symbol_table_exit_scope(&table);
    assert(
        symbol_table_lookup(&table, "y")
        == NULL
    );

    for (int i = 0; i < 20; i++) {
        char name[32];
        snprintf(name, sizeof(name), "variable_%d", i);
        Symbol *symbol = new_symbol(
            name, TYPE_INT, 0
        );
        assert(
            symbol_table_insert(&table, symbol)
            == SYMBOL_TABLE_OK
        );
        assert(
            symbol_table_lookup(&table, name)
            == symbol
        );
    }

    assert(table.current_scope->count == 21);
    assert(table.current_scope->capacity >= 21);
    assert(
        symbol_table_insert(&table, NULL)
        == SYMBOL_TABLE_INTERNAL_ERROR
    );
    assert(
        symbol_table_lookup_current(&table, NULL)
        == NULL
    );
    assert(
        symbol_table_lookup(&table, "missing")
        == NULL
    );

    symbol_table_destroy(&table);
    assert(table.current_scope == NULL);
    assert(table.owned_scopes == NULL);

    symbol_table_destroy(&table);
    SymbolTable empty = {0};
    symbol_table_destroy(&empty);

    printf("[OK] SymbolTable\n");

    return 0;
}
