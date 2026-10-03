#!/usr/bin/env bash

EXEC=${1:-./minic}

PASSED=0
FAILED=0

run_test() {
    file="$1"
    expected_exit="$2"

    "$EXEC" "$file" > /tmp/minic_custom_stdout.txt 2> /tmp/minic_custom_stderr.txt
    actual_exit=$?

    name=$(basename "$file" .mc)

    if [ "$actual_exit" -eq "$expected_exit" ]; then
        echo "[OK] $name"
        PASSED=$((PASSED + 1))
    else
        echo "[FALLO] $name"
        echo "  Esperado exit: $expected_exit"
        echo "  Obtenido exit: $actual_exit"

        echo "  stdout:"
        cat /tmp/minic_custom_stdout.txt

        echo "  stderr:"
        cat /tmp/minic_custom_stderr.txt

        FAILED=$((FAILED + 1))
    fi
}

run_test tests/parser_custom/inputs/c01_empty_block.mc 0
run_test tests/parser_custom/inputs/c02_nested_control.mc 0
run_test tests/parser_custom/inputs/c03_if_without_block.mc 0
run_test tests/parser_custom/inputs/c04_while_without_block.mc 0
run_test tests/parser_custom/inputs/c05_no_semantic_checks.mc 0

run_test tests/parser_custom/inputs/c06_multiple_recovery.mc 1
run_test tests/parser_custom/inputs/c07_lonely_else.mc 1
run_test tests/parser_custom/inputs/c08_missing_brace_before_else.mc 1

run_test tests/parser_custom/inputs/c09_unary_chain.mc 0
run_test tests/parser_custom/inputs/c10_full_precedence.mc 0

run_test tests/parser_custom/inputs/c11_expression_eof.mc 1
run_test tests/parser_custom/inputs/c12_lexical_and_syntax.mc 1

rm -f /tmp/minic_custom_stdout.txt
rm -f /tmp/minic_custom_stderr.txt

echo
echo "Aprobadas: $PASSED"
echo "Fallidas: $FAILED"

if [ "$FAILED" -ne 0 ]; then
    exit 1
fi

exit 0
