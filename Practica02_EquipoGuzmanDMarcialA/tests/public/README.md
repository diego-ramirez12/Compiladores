# Pruebas

Este directorio contiene los casos utilizados para verificar el funcionamiento del analizador léxico desarrollado durante las Prácticas 1 y 2.

Las pruebas permiten comprobar tanto las funcionalidades heredadas de la Práctica 1 como las incorporadas en la Práctica 2: identificadores, palabras reservadas, literales booleanos, operadores compuestos, comentarios, recuperación ante errores léxicos y seguimiento correcto de línea y columna.

Las pruebas se dividen en dos conjuntos:

```text
tests/
├── public/
│   ├── README.md
│   ├── inputs/
│   ├── expected/
│   └── run_public_tests.sh
└── custom/
    ├── inputs/
    └── expected/
```

## Ejecución

Todas las pruebas pueden ejecutarse desde la raíz del proyecto mediante:

```text
make test
```

El `Makefile` ejecuta cada archivo `.mc` con el programa `minic` y compara la salida obtenida con el archivo `.out` correspondiente mediante `diff`.

Por ejemplo:

```text
tests/public/inputs/p01_identifiers.mc
```

se compara con:

```text
tests/public/expected/p01_identifiers.out
```

Si ambas salidas coinciden exactamente, se muestra:

```text
public/p01_identifiers: OK
```

## Pruebas públicas

El directorio `public/` contiene las pruebas proporcionadas con el material de las Prácticas 1 y 2.

Se conservan las diez pruebas públicas de la Práctica 1 y se incorporan trece pruebas públicas correspondientes a la Práctica 2.

### Pruebas públicas de la Práctica 1

| Prueba | Propósito principal |
|---|---|
| `p1_empty` | Verificar un archivo vacío y la posición inicial de `TOKEN_EOF`. |
| `p2_whitespace` | Verificar el manejo de espacios en blanco, tabuladores y saltos de línea. |
| `p3_single_symbols` | Verificar todos los símbolos simples requeridos en la Práctica 1. |
| `p4_integer` | Verificar el reconocimiento de un entero de varios dígitos. |
| `p5_integer_expression` | Verificar enteros y símbolos combinados en una misma línea. |
| `p6_multiple_lines` | Verificar el seguimiento de línea y columna en varias líneas. |
| `p7_tabs_spaces` | Verificar la actualización de columnas al encontrar espacios y tabuladores. |
| `p8_error_character` | Verificar la generación de `ERROR` y la continuación del análisis. |
| `p9_negative_number` | Verificar que el signo negativo sea reconocido como `MINUS` separado de `INTEGER`. |
| `p10_mixed_basic` | Verificar una combinación de distintos elementos léxicos en varias líneas. |

### Pruebas públicas de la Práctica 2

| Prueba | Propósito principal |
|---|---|
| `p01_identifiers` | Verificar identificadores simples, con dígitos y con guiones bajos. |
| `p02_reserved_words` | Verificar el reconocimiento de las palabras reservadas de MiniC. |
| `p03_case_sensitive` | Verificar la distinción entre mayúsculas y minúsculas. |
| `p04_booleans` | Verificar el reconocimiento de los literales booleanos `true` y `false`. |
| `p05_compound_operators` | Verificar los operadores compuestos `==`, `!=`, `<=`, `>=`, `&&` y `||`. |
| `p06_incomplete_operators` | Verificar que `!`, `&` y `|` aislados produzcan `ERROR`. |
| `p07_comments` | Verificar que los comentarios de una línea sean ignorados correctamente. |
| `p08_comment_at_eof` | Verificar un comentario que termina directamente al final del archivo. |
| `p09_slash_and_comments` | Verificar la distinción entre el operador `/` y el inicio de comentario `//`. |
| `p10_longest_match` | Verificar la selección del lexema válido de mayor longitud. |
| `p11_error_recovery` | Verificar que el análisis continúe después de detectar errores léxicos. |
| `p12_complete_lexer` | Verificar conjuntamente las principales funcionalidades del lexer completo. |
| `p13_crlf` | Verificar el manejo correcto de CRLF y el seguimiento de posición. |

## Pruebas propias

El directorio `custom/` contiene diez casos adicionales desarrollados por el equipo: cuatro correspondientes a la Práctica 1 y seis incorporados durante la Práctica 2.

### `c1_multiple_errors`

Entrada:

```text
@#$
```

Salida esperada:

```text
1:0 ERROR @
1:1 ERROR #
1:2 ERROR $
1:3 TOKEN_EOF
```

Verifica que cada carácter no reconocido genere su propio token `ERROR` y que el lexer continúe procesando la entrada.

### `c2_long_integer`

Entrada:

```text
123456789012345678901234567890;
```

Salida esperada:

```text
1:0 INTEGER 123456789012345678901234567890
1:30 SEMICOLON ;
1:31 TOKEN_EOF
```

Verifica que una secuencia larga de dígitos se reconozca como un único `INTEGER` y que el almacenamiento dinámico del lexema pueda crecer cuando sea necesario.

### `c3_isolated_cr`

La entrada contiene:

```text
1\r2
```

donde `\r` representa un retorno de carro aislado.

Salida esperada:

```text
1:0 INTEGER 1
2:0 INTEGER 2
2:1 TOKEN_EOF
```

Verifica que un retorno de carro aislado sea tratado como un salto de línea.

### `c4_crlf`

La entrada contiene:

```text
1\r\n2
```

donde `\r\n` representa una secuencia CRLF.

Salida esperada:

```text
1:0 INTEGER 1
2:0 INTEGER 2
2:1 TOKEN_EOF
```

Verifica que `\r\n` sea contabilizado como un único salto de línea.

### `c5_identifier_edges`

Entrada:

```text
_ _1 abc123 while1 While true_value 1contador
```

Salida esperada:

```text
1:0 IDENTIFIER _
1:2 IDENTIFIER _1
1:5 IDENTIFIER abc123
1:12 IDENTIFIER while1
1:19 IDENTIFIER While
1:25 IDENTIFIER true_value
1:36 INTEGER 1
1:37 IDENTIFIER contador
2:0 TOKEN_EOF
```

Verifica casos límite de identificadores, prefijos de palabras reservadas y la separación entre enteros e identificadores.

### `c6_operator_sequences`

Entrada:

```text
=== !== <== >== &&& ||| ! & |
```

Salida esperada:

```text
1:0 EQUAL ==
1:2 ASSIGN =
1:4 NOT_EQUAL !=
1:6 ASSIGN =
1:8 LESS_EQUAL <=
1:10 ASSIGN =
1:12 GREATER_EQUAL >=
1:14 ASSIGN =
1:16 AND &&
1:18 ERROR &
1:20 OR ||
1:22 ERROR |
1:24 ERROR !
1:26 ERROR &
1:28 ERROR |
2:0 TOKEN_EOF
```

Verifica la prioridad de reconocimiento de operadores compuestos y el tratamiento de operadores incompletos.

### `c7_comment_content`

Entrada:

```text
int x = 10; // while != && @ true
print(x); //final
```

Salida esperada:

```text
1:0 INT int
1:4 IDENTIFIER x
1:6 ASSIGN =
1:8 INTEGER 10
1:10 SEMICOLON ;
2:0 PRINT print
2:5 LPAREN (
2:6 IDENTIFIER x
2:7 RPAREN )
2:8 SEMICOLON ;
3:0 TOKEN_EOF
```

Verifica que todo el contenido posterior a `//` sea ignorado hasta el final de la línea, incluso si contiene palabras reservadas, operadores o caracteres inválidos.

### `c8_number_identifier`

Entrada:

```text
123abc 0_var 42while1 true2
```

Salida esperada:

```text
1:0 INTEGER 123
1:3 IDENTIFIER abc
1:7 INTEGER 0
1:8 IDENTIFIER _var
1:13 INTEGER 42
1:15 IDENTIFIER while1
1:22 IDENTIFIER true2
2:0 TOKEN_EOF
```

Verifica la separación correcta entre enteros e identificadores cuando aparecen consecutivamente.

### `c9_p2_error_recovery`

Entrada:

```text
! != & && | || @valor
```

Salida esperada:

```text
1:0 ERROR !
1:2 NOT_EQUAL !=
1:5 ERROR &
1:7 AND &&
1:10 ERROR |
1:12 OR ||
1:15 ERROR @
1:16 IDENTIFIER valor
2:0 TOKEN_EOF
```

Verifica la recuperación ante errores junto con los operadores incorporados en la Práctica 2.

### `c10_comment_isolated_cr`

La entrada contiene un comentario terminado mediante un retorno de carro aislado:

```text
// comentario\rint x;
```

donde `\r` representa un carácter real de retorno de carro.

Salida esperada:

```text
2:0 INT int
2:4 IDENTIFIER x
2:5 SEMICOLON ;
2:6 TOKEN_EOF
```

Verifica que un retorno de carro aislado termine correctamente un comentario, actualice la línea y permita continuar el análisis desde la columna cero.

## Resultado esperado

Al ejecutar:

```text
make test
```

todas las pruebas públicas y propias deben finalizar con:

```text
OK
```

En total se ejecutan:

- 23 pruebas públicas:
  - 10 heredadas de la Práctica 1.
  - 13 incorporadas en la Práctica 2.
- 10 pruebas propias:
  - 4 desarrolladas en la Práctica 1.
  - 6 incorporadas en la Práctica 2.
- 33 pruebas en total.

Las salidas deben coincidir exactamente con los archivos almacenados en los directorios `expected/`.