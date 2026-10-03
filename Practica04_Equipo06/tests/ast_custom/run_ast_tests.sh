#!/bin/sh

EXECUTABLE="${1:-./minic}"

PASSED=0
FAILED=0

for input in tests/ast_custom/inputs/*.mc
do
    name=$(basename "$input" .mc)

    expected="tests/ast_custom/expected/$name.out"
    actual="tests/ast_custom/actual.out"
    error="tests/ast_custom/error.out"

    "$EXECUTABLE" "$input" > "$actual" 2> "$error"
    status=$?

    if [ "$status" -eq 0 ] &&
       [ ! -s "$error" ] &&
       diff -u "$expected" "$actual" > /dev/null
    then
        echo "[OK] $name"
        PASSED=$((PASSED + 1))
    else
        echo "[FAIL] $name"
        FAILED=$((FAILED + 1))

        echo "--- Diferencias ---"
        diff -u "$expected" "$actual"

        if [ -s "$error" ]; then
            echo "--- stderr ---"
            cat "$error"
        fi
    fi
done

rm -f tests/ast_custom/actual.out
rm -f tests/ast_custom/error.out

echo
echo "Aprobadas: $PASSED"
echo "Fallidas: $FAILED"

if [ "$FAILED" -ne 0 ]; then
    exit 1
fi

exit 0
