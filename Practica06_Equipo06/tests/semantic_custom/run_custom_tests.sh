set -u

EXECUTABLE="${1:-./minic}"

if [[ ! -x "$EXECUTABLE" ]]; then
    echo "No se encontro el ejecutable: $EXECUTABLE" >&2
    exit 2
fi
TMP_DIR=$(mktemp -d) || exit 2
trap 'rm -rf "$TMP_DIR"' EXIT
PASSED=0
FAILED=0

# Verifica un programa semanticamente valido.
valid_case() {
    local name="$1" input="$2" status

    "$EXECUTABLE" "$input" \
        >"$TMP_DIR/stdout" \
        2>"$TMP_DIR/stderr"
    status=$?
    if [[ $status -eq 0 ]] &&
       [[ ! -s "$TMP_DIR/stderr" ]] &&
       printf 'Programa semanticamente correcto.\n' |
           cmp -s - "$TMP_DIR/stdout"; then

        echo "[OK] $name"
        PASSED=$((PASSED + 1))
    else
        echo "[FALLO] $name"
        cat "$TMP_DIR/stdout" "$TMP_DIR/stderr"
        FAILED=$((FAILED + 1))
    fi
}

# Verifica los errores esperados y sus posiciones.
invalid_case() {
    local name="$1" input="$2" expected="$3"
    local status errors pattern ok=1
    shift 3

    "$EXECUTABLE" "$input" \
        >"$TMP_DIR/stdout" \
        2>"$TMP_DIR/stderr"

    status=$?
    errors=$(grep -c '^Error semantico ' "$TMP_DIR/stderr" || true)

    if [[ $status -ne 1 ]] ||
       [[ -s "$TMP_DIR/stdout" ]] ||
       [[ $errors -ne $expected ]]; then
        ok=0
    fi
    for pattern in "$@"; do
        if ! grep -Fq "$pattern" "$TMP_DIR/stderr"; then
            ok=0
        fi
    done
    if [[ $ok -eq 1 ]]; then
        echo "[OK] $name"
        PASSED=$((PASSED + 1))
    else
        echo "[FALLO] $name"
        printf 'Codigo: %s | Errores: %s\n' "$status" "$errors"
        cat "$TMP_DIR/stdout" "$TMP_DIR/stderr"
        FAILED=$((FAILED + 1))
    fi
}

# Caso 1: shadowing en tres niveles.
cat >"$TMP_DIR/shadowing.mc" <<'MC'
int x;
{
    bool x;
    {
        int x;
        x = 1;
    }
    x = true;
}
x = 4;
MC

valid_case shadowing_multinivel "$TMP_DIR/shadowing.mc"

# Caso 2: redeclaracion y variables no declaradas.
cat >"$TMP_DIR/errores.mc" <<'MC'
int x;
bool x = y;
{
    int z;
    print(w);
    z = 1;
}
print(q);
MC

invalid_case errores_acumulados "$TMP_DIR/errores.mc" 4 \
    "[2:5]: la variable 'x' ya fue declarada" \
    "[2:9]: la variable 'y' no ha sido declarada" \
    "[5:10]: la variable 'w' no ha sido declarada" \
    "[8:6]: la variable 'q' no ha sido declarada"

# Caso 3: variable utilizada despues de salir del bloque.
cat >"$TMP_DIR/fuera.mc" <<'MC'
int g;
{
    int x;
    g = x;
}
x = 1;
MC

invalid_case uso_fuera_de_ambito "$TMP_DIR/fuera.mc" 1 \
    "[6:0]: la variable 'x' no ha sido declarada"

# Caso 4: mismo nombre en bloques hermanos.
cat >"$TMP_DIR/hermanos.mc" <<'MC'
{
    int t;
}
{
    int t;
    t = 2;
}
MC

valid_case bloques_hermanos "$TMP_DIR/hermanos.mc"

# Caso 5: no verificar tipos en P05.
cat >"$TMP_DIR/tipos.mc" <<'MC'
int n;
bool b;
n = b;
if (n) {
    print(b);
}
MC

valid_case tipos_pospuestos_p06 "$TMP_DIR/tipos.mc"

# Caso 6: identificadores no declarados en expresiones.
cat >"$TMP_DIR/expresion.mc" <<'MC'
int x;
x = -a + b;
MC

invalid_case operandos_no_declarados "$TMP_DIR/expresion.mc" 2 \
    "[2:5]: la variable 'a' no ha sido declarada" \
    "[2:9]: la variable 'b' no ha sido declarada"

# Caso 7: 300 variables para probar crecimiento dinamico.
{
    for i in $(seq 0 299); do
        printf 'int variable_%d;\n' "$i"
    done

    printf '{\n'
    printf '    bool variable_150;\n'
    printf '    variable_150 = true;\n'
    printf '    variable_0 = 2;\n'
    printf '}\n'
    printf 'variable_299 = 10;\n'
} >"$TMP_DIR/muchas.mc"

valid_case trescientas_variables "$TMP_DIR/muchas.mc"

echo
printf 'Aprobadas: %d | Fallidas: %d\n' "$PASSED" "$FAILED"

[[ "$FAILED" -eq 0 ]]
