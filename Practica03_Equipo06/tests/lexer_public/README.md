# Pruebas públicas — Regresión del lexer de las Prácticas 1 y 2

## Uso

Estas pruebas permiten verificar que la adaptación del lexer a una interfaz incremental en la Práctica 3 conserve el comportamiento implementado durante las Prácticas 1 y 2.

El ejecutable normal `minic` ya no imprime la secuencia de tokens, debido a que en la Práctica 3 éstos son consumidos internamente por el parser. Por esta razón, las pruebas de regresión utilizan `tests/lexer_driver.c`, que solicita los tokens mediante `lexer_next_token()` y los imprime con el formato empleado en las prácticas anteriores.

Desde la raíz del proyecto, las pruebas públicas del lexer pueden ejecutarse mediante:

```text
make test-lexer
```

Esta regla compila temporalmente el ejecutable:

```text
lexer_test
```

y posteriormente ejecuta:

```text
bash tests/lexer_public/run_public_tests.sh ./lexer_test
```

El script compara la salida producida para cada archivo de `inputs/` con el archivo correspondiente dentro de `expected/`.

## Pruebas heredadas de la Práctica 1

| Entrada | Propósito principal |
|---|---|
| `p1_empty.mc` | Archivo vacío y posición inicial de `TOKEN_EOF`. |
| `p2_whitespace.mc` | Espacios, tabuladores y saltos de línea. |
| `p3_single_symbols.mc` | Todos los símbolos simples requeridos en la Práctica 1. |
| `p4_integer.mc` | Entero de varios dígitos. |
| `p5_integer_expression.mc` | Enteros y símbolos en una línea. |
| `p6_multiple_lines.mc` | Seguimiento de línea y columna en varias líneas. |
| `p7_tabs_spaces.mc` | Columnas con tabuladores y espacios. |
| `p8_error_character.mc` | Generación de `ERROR` y recuperación. |
| `p9_negative_number.mc` | Separación entre `MINUS` e `INTEGER`. |
| `p10_mixed_basic.mc` | Combinación básica de elementos léxicos. |

## Pruebas incorporadas en la Práctica 2

| Entrada | Propósito principal |
|---|---|
| `p01_identifiers.mc` | Identificadores de uno o varios caracteres, con dígitos y guiones bajos. |
| `p02_reserved_words.mc` | Palabras reservadas de MiniC. |
| `p03_case_sensitive.mc` | Distinción entre mayúsculas y minúsculas. |
| `p04_booleans.mc` | Literales booleanos `true` y `false`. |
| `p05_compound_operators.mc` | Operadores `==`, `!=`, `<=`, `>=`, `&&` y `||`. |
| `p06_incomplete_operators.mc` | `!`, `&` y `|` aislados como `ERROR`. |
| `p07_comments.mc` | Comentarios de una línea después de otros tokens. |
| `p08_comment_at_eof.mc` | Comentario que termina al final del archivo. |
| `p09_slash_and_comments.mc` | Distinción entre `/` y `//`. |
| `p10_longest_match.mc` | Reconocimiento del lexema válido de mayor longitud. |
| `p11_error_recovery.mc` | Continuación del análisis después de errores léxicos. |
| `p12_complete_lexer.mc` | Integración de las principales funcionalidades del lexer completo. |
| `p13_crlf.mc` | Seguimiento correcto de posición con secuencias CRLF. |

## Alcance

En este proyecto se conservan las diez pruebas públicas de la Práctica 1 y las trece pruebas públicas incorporadas en la Práctica 2, para un total de 23 casos de regresión del lexer.

Estas pruebas permiten comprobar que la nueva interfaz incremental utilizada en la Práctica 3 no altere el reconocimiento de tokens, lexemas ni posiciones desarrollado anteriormente.

Las pruebas propias del lexer se conservan por separado en:

```text
tests/lexer_custom/
```

Las pruebas públicas y propias del lexer forman parte de la validación de regresión del proyecto y no sustituyen las pruebas correspondientes al parser de la Práctica 3.