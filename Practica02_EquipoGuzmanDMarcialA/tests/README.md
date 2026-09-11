# Pruebas

Este directorio contiene los casos utilizados para verificar el funcionamiento del analizador léxico desarrollado para la Práctica 1.

Las pruebas se dividen en dos conjuntos:

```text
tests/
├── public/
│   ├── inputs/
│   └── expected/
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
tests/public/inputs/p5_integer_expression.mc
```

se compara con:

```text
tests/public/expected/p5_integer_expression.out
```

Si ambas salidas coinciden exactamente, se muestra:

```text
public/p5_integer_expression: OK
```

## Pruebas públicas

El directorio `public/` contiene los diez casos proporcionados con el material de la práctica.

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

## Pruebas propias

El directorio `custom/` contiene cuatro casos adicionales desarrollados por el equipo.

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

Verifica que una secuencia larga de dígitos se reconozca como un único token `INTEGER` y que el almacenamiento dinámico del lexema pueda crecer cuando sea necesario.

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

Verifica que un retorno de carro aislado (`\r`) sea tratado como un salto de línea. No genera un token, pero incrementa la línea y reinicia la columna a cero.

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

Verifica que la secuencia `\r\n` sea considerada un único salto de línea. Aunque está formada por dos caracteres, la línea debe incrementarse solamente una vez.

## Resultado esperado

Al ejecutar:

```text
make test
```

todas las pruebas públicas y propias deben finalizar con el indicador:

```text
OK
```

En total se ejecutan:

- 10 pruebas públicas.
- 4 pruebas propias.
- 14 pruebas en total.

Las salidas deben coincidir exactamente con los archivos almacenados en los directorios `expected/`.