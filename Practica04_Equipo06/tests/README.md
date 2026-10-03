# Pruebas

Este directorio contiene los casos utilizados para verificar la construcción del Árbol de Sintaxis Abstracta de la Práctica 4, así como las pruebas de regresión del analizador sintáctico y del analizador léxico desarrollados en las prácticas anteriores.

Las pruebas se encuentran organizadas de la siguiente forma:

```text
tests/
├── README.md
├── lexer_driver.c
├── public/
│   ├── README.md
│   ├── inputs/
│   ├── expected/
│   └── run_public_tests.sh
├── ast_custom/
│   ├── inputs/
│   ├── expected/
│   └── run_ast_tests.sh
├── parser_custom/
│   ├── inputs/
│   └── run_custom_tests.sh
├── lexer_public/
│   ├── README.md
│   ├── inputs/
│   ├── expected/
│   └── run_public_tests.sh
├── lexer_custom/
│   ├── inputs/
│   └── expected/
└── public_p3/
    ├── README.md
    ├── inputs/
    ├── expected/
    └── run_public_tests.sh
```

## Ejecución

Los comandos deben ejecutarse desde la raíz del proyecto.

### Pruebas públicas de la Práctica 4

```text
make test
```

Ejecuta las pruebas públicas correspondientes a la Práctica 4.

Estas pruebas verifican tanto la aceptación de programas MiniC como la estructura y el formato canónico del AST producido.

### Pruebas propias del AST

```text
make test-ast
```

Ejecuta los casos adicionales desarrollados por el equipo para comprobar la estructura del Árbol de Sintaxis Abstracta.

### Pruebas propias del parser

```text
make test-custom
```

Ejecuta los casos adicionales desarrollados durante la Práctica 3 para verificar que la incorporación del AST no haya modificado el reconocimiento sintáctico ni la recuperación ante errores.

### Pruebas de regresión del lexer

```text
make test-lexer
```

Compila `tests/lexer_driver.c` junto con el lexer actual y ejecuta las 23 pruebas públicas heredadas de las Prácticas 1 y 2.

### Ejecución conjunta

```text
make test-all
```

Ejecuta:

1. las pruebas públicas de la Práctica 4;
2. las pruebas propias del parser;
3. las pruebas públicas de regresión del lexer;
4. las pruebas propias del AST.

Actualmente la ejecución conjunta verifica un total de 53 casos:

```text
Pruebas públicas P4:      12/12
Pruebas propias parser:   12/12
Pruebas lexer:            23/23
Pruebas propias AST:       6/6
--------------------------------
Total:                    53/53
```

## Pruebas públicas de la Práctica 4

El directorio `public/` contiene las 12 pruebas proporcionadas para la Práctica 4.

Estas pruebas verifican casos relacionados con:

- programa vacío;
- declaraciones;
- declaraciones con inicialización;
- asignaciones;
- sentencias `print`;
- precedencia de expresiones;
- operadores unarios;
- expresiones agrupadas mediante paréntesis;
- condicionales `if` y `else`;
- ciclos `while`;
- bloques anidados;
- programas completos;
- errores sintácticos;
- errores léxicos.

Para los programas válidos, las pruebas comparan exactamente la salida producida en `stdout` con el AST esperado.

Por ejemplo, una salida válida tiene la forma:

```text
AST:
Program
  VariableDeclaration(int, x)
    Integer(5)
Programa sintacticamente correcto.
```

Las pruebas también verifican que:

- el AST utilice exactamente dos espacios por nivel;
- las sentencias aparezcan en el orden correcto;
- los operandos binarios aparezcan primero a la izquierda y después a la derecha;
- la precedencia y asociatividad estén representadas mediante la estructura del árbol;
- no aparezcan nodos correspondientes a paréntesis, puntos y coma u otros elementos de sintaxis concreta;
- los programas inválidos no impriman un AST parcial;
- los programas inválidos terminen con un código distinto de cero.

La descripción detallada de cada caso se encuentra en:

```text
tests/public/README.md
```

## Pruebas propias del AST

El directorio `ast_custom/` contiene seis pruebas adicionales desarrolladas por el equipo.

Estas pruebas comparan la salida exacta generada por `minic` contra un archivo esperado.

| Prueba | Resultado esperado | Propósito principal |
|---|---|---|
| `a01_left_associativity` | Válido | Verificar la asociatividad izquierda mediante `10 - 5 - 2`. |
| `a02_full_precedence` | Válido | Verificar conjuntamente todos los niveles principales de precedencia. |
| `a03_unary_chain` | Válido | Verificar operadores negativos unarios consecutivos mediante `--x`. |
| `a04_empty_block` | Válido | Verificar la construcción e impresión de un bloque vacío. |
| `a05_nested_if` | Válido | Verificar la construcción de condicionales anidados con rama `else`. |
| `a06_nested_while` | Válido | Verificar la construcción de ciclos `while` anidados. |

El script:

```text
tests/ast_custom/run_ast_tests.sh
```

ejecuta cada entrada y comprueba:

- que el código de salida sea `0`;
- que `stderr` permanezca vacío;
- que la salida producida coincida exactamente con el archivo esperado.

Estas pruebas permiten comprobar de manera adicional que la estructura del AST sea correcta y no solamente que el programa sea aceptado por el parser.

## Pruebas propias del parser

El directorio `parser_custom/` contiene 12 casos adicionales desarrollados durante la Práctica 3.

Estas pruebas se conservan como pruebas de regresión para comprobar que la incorporación del AST no haya modificado el comportamiento sintáctico existente.

| Prueba | Resultado esperado | Propósito principal |
|---|---|---|
| `c01_empty_block` | Válido | Verificar un bloque vacío. |
| `c02_nested_control` | Válido | Verificar estructuras de control anidadas. |
| `c03_if_without_block` | Válido | Verificar un `if` cuyo cuerpo es una sentencia individual. |
| `c04_while_without_block` | Válido | Verificar un `while` cuyo cuerpo es una sentencia individual. |
| `c05_no_semantic_checks` | Válido | Confirmar que no se realizan comprobaciones semánticas. |
| `c06_multiple_recovery` | Inválido | Verificar la continuación del análisis después de varios errores. |
| `c07_lonely_else` | Inválido | Detectar un `else` sin un `if` asociado. |
| `c08_missing_brace_before_else` | Inválido | Detectar una llave de cierre faltante antes de un `else`. |
| `c09_unary_chain` | Válido | Verificar varios operadores negativos unarios consecutivos. |
| `c10_full_precedence` | Válido | Verificar la combinación de los niveles de precedencia de operadores. |
| `c11_expression_eof` | Inválido | Detectar una expresión incompleta al llegar a `TOKEN_EOF`. |
| `c12_lexical_and_syntax` | Inválido | Verificar la continuación del análisis ante errores léxicos y sintácticos. |

El script `run_custom_tests.sh` ejecuta cada archivo y comprueba que el código de salida obtenido corresponda con el esperado para un programa válido o inválido.

Estas pruebas no comparan la salida canónica completa del AST. Su propósito principal es servir como regresión del comportamiento sintáctico desarrollado en la Práctica 3.

## Pruebas de regresión del lexer

El directorio `lexer_public/` conserva las pruebas públicas utilizadas durante las Prácticas 1 y 2.

Se mantienen:

- 10 pruebas públicas heredadas de la Práctica 1;
- 13 pruebas públicas incorporadas en la Práctica 2;
- 23 pruebas públicas de regresión en total.

Para ejecutar estas pruebas con la interfaz incremental actual del lexer se utiliza:

```text
tests/lexer_driver.c
```

Este programa solicita los tokens mediante la interfaz actual del lexer y los imprime con el formato utilizado en las prácticas anteriores.

De esta forma se comprueba que la incorporación del parser y posteriormente del AST no haya modificado el comportamiento léxico previamente desarrollado.

La descripción detallada de estas pruebas se encuentra en:

```text
tests/lexer_public/README.md
```

## Pruebas propias del lexer

El directorio `lexer_custom/` conserva las 10 pruebas propias desarrolladas durante las Prácticas 1 y 2.

Estas pruebas cubren casos como:

- múltiples caracteres inválidos;
- enteros largos;
- saltos de línea mediante `\r` y `\r\n`;
- casos límite de identificadores;
- secuencias de operadores;
- contenido de comentarios;
- separación entre números e identificadores;
- recuperación después de errores léxicos.

Los archivos se conservan como parte de las pruebas de regresión del lexer.

El `Makefile` actual no incluye una regla específica para ejecutar automáticamente este conjunto.

## Pruebas públicas de la Práctica 3

El directorio:

```text
tests/public_p3/
```

conserva como respaldo las 17 pruebas públicas utilizadas durante la Práctica 3.

Estas pruebas no forman parte de `make test` ni de `make test-all` en la Práctica 4.

Las pruebas públicas originales de la Práctica 3 no pueden utilizarse sin modificaciones para comprobar la salida de los programas válidos de la Práctica 4.

En la Práctica 3, un programa válido producía únicamente:

```text
Programa sintacticamente correcto.
```

En la Práctica 4, el mismo programa debe producir:

```text
AST:
Program
  ...
Programa sintacticamente correcto.
```

Por esta razón, las salidas esperadas de las pruebas públicas de la Práctica 3 ya no coinciden con el contrato de salida vigente.

Esto no significa que las funcionalidades anteriores hayan dejado de funcionar.

El reconocimiento sintáctico de la Práctica 3 continúa siendo utilizado por la Práctica 4 y se comprueba mediante:

```text
tests/parser_custom/
```

Además, las funcionalidades del lexer de las Prácticas 1 y 2 continúan verificándose mediante:

```text
tests/lexer_public/
```

El directorio `public_p3/` se mantiene únicamente como referencia y respaldo de las pruebas de la práctica anterior.

## Compatibilidad con prácticas anteriores

La Práctica 4 conserva las funcionalidades implementadas anteriormente, pero no todas las suites antiguas pueden ejecutarse sin cambios debido a que el formato de salida del ejecutable evolucionó.

La situación es la siguiente:

```text
Pruebas públicas P4       -> Sí, son las pruebas principales actuales.
Pruebas propias AST       -> Sí.
Pruebas propias parser P3 -> Sí, se utilizan como regresión.
Pruebas públicas lexer    -> Sí, se utilizan como regresión.
Pruebas propias lexer     -> Se conservan como referencia.
Pruebas públicas P3       -> No se ejecutan sin modificaciones debido al cambio de stdout.
```

Por lo tanto, para verificar la implementación actual debe utilizarse:

```text
make test-all
```

y no el script almacenado en `tests/public_p3/`.

## Pruebas de memoria

Además de las pruebas funcionales, la implementación fue verificada mediante Valgrind.

Se probaron:

- programas válidos;
- programas con errores sintácticos;
- programas con errores léxicos.

Por ejemplo:

```text
valgrind --leak-check=full \
         --show-leak-kinds=all \
         ./minic tests/public/inputs/p10_complete_program.mc
```

El resultado esperado es:

```text
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

Las pruebas realizadas confirmaron que los nodos del AST y los recursos del lexer y parser se liberan correctamente tanto en programas válidos como inválidos.

## Resultado esperado

Las pruebas públicas de la Práctica 4 deben verificar tanto que el programa sea aceptado como que el AST tenga exactamente la estructura esperada.

Las pruebas propias del AST deben producir exactamente la misma salida almacenada en sus archivos `expected`.

Las pruebas propias del parser deben clasificar correctamente cada archivo como válido o inválido.

Las pruebas públicas de regresión del lexer deben producir exactamente la misma secuencia de tokens esperada para las funcionalidades implementadas en las Prácticas 1 y 2.

La ejecución conjunta puede realizarse mediante:

```text
make test-all
```

y debe finalizar sin pruebas fallidas.

El resultado actual es:

```text
Pruebas públicas P4:      12/12
Pruebas propias parser:   12/12
Pruebas lexer:            23/23
Pruebas propias AST:       6/6
--------------------------------
Total:                    53/53
```