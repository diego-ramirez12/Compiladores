# Pruebas públicas — Prácticas 1 y 2

## Uso

Cada archivo de `inputs/` debe ejecutarse con el programa `minic`. La salida estándar debe coincidir exactamente con el archivo del mismo nombre ubicado en `expected/`, cambiando la extensión `.mc` por `.out`.

Desde la raíz del proyecto, todas las pruebas públicas y propias pueden ejecutarse mediante:

```text
make test
```

El material de la Práctica 2 también incluye el script:

```text
sh tests/public/run_public_tests.sh
```

Los diagnósticos operativos no deben mezclarse con la secuencia de tokens en `stdout`.

## Pruebas heredadas de la Práctica 1

| Entrada | Propósito principal |
|---|---|
| `p1_empty.mc` | Archivo vacío y posición inicial de `TOKEN_EOF`. |
| `p2_whitespace.mc` | Espacios, tabuladores y saltos de línea. |
| `p3_single_symbols.mc` | Todos los símbolos simples requeridos en P1. |
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
| `p13_crlf.mc` | Seguimiento correcto de posición con CRLF. |

## Alcance

En este proyecto se conservan las diez pruebas públicas de la Práctica 1 y se añaden las trece pruebas públicas de la Práctica 2, para un total de 23 casos públicos.

Estas pruebas representan el comportamiento mínimo esperado. El proyecto también incluye pruebas propias en `tests/custom/`, ya que durante la evaluación pueden utilizarse casos adicionales que respeten la especificación de MiniC.