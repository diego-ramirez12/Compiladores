# Pruebas

Este directorio contiene los casos utilizados para verificar el analizador sintáctico de la Práctica 3 y las pruebas de regresión del analizador léxico desarrollado en las Prácticas 1 y 2.

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
├── parser_custom/
│   ├── inputs/
│   └── run_custom_tests.sh
├── lexer_public/
│   ├── README.md
│   ├── inputs/
│   ├── expected/
│   └── run_public_tests.sh
└── lexer_custom/
    ├── inputs/
    └── expected/
```

## Ejecución

Los comandos deben ejecutarse desde la raíz del proyecto.

### Pruebas públicas del parser

```text
make test
```

Ejecuta las pruebas públicas correspondientes a la Práctica 3.

### Pruebas propias del parser

```text
make test-custom
```

Ejecuta los casos adicionales desarrollados por el equipo.

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

1. las pruebas públicas del parser;
2. las pruebas propias del parser;
3. las pruebas públicas de regresión del lexer.

## Pruebas públicas del parser

El directorio `public/` contiene las 17 pruebas proporcionadas para la Práctica 3.

Estas pruebas verifican casos válidos e inválidos relacionados con:

- declaraciones;
- asignaciones;
- sentencias `print`;
- precedencia de expresiones;
- condicionales y asociación de `else`;
- ciclos `while`;
- bloques;
- errores por símbolos faltantes;
- expresiones incompletas;
- tokens inesperados;
- recuperación ante múltiples errores;
- errores léxicos recibidos por el parser;
- consumo completo de la entrada.

La descripción detallada de cada caso se encuentra en:

```text
tests/public/README.md
```

## Pruebas propias del parser

El directorio `parser_custom/` contiene 12 casos adicionales desarrollados por el equipo.

| Prueba | Resultado esperado | Propósito principal |
|---|---|---|
| `c01_empty_block` | Válido | Verificar un bloque vacío. |
| `c02_nested_control` | Válido | Verificar estructuras de control anidadas. |
| `c03_if_without_block` | Válido | Verificar un `if` cuyo cuerpo es una sentencia individual. |
| `c04_while_without_block` | Válido | Verificar un `while` cuyo cuerpo es una sentencia individual. |
| `c05_no_semantic_checks` | Válido | Confirmar que el parser valida sintaxis y no realiza comprobaciones semánticas. |
| `c06_multiple_recovery` | Inválido | Verificar la continuación del análisis después de varios errores. |
| `c07_lonely_else` | Inválido | Detectar un `else` sin un `if` asociado. |
| `c08_missing_brace_before_else` | Inválido | Detectar una llave de cierre faltante antes de un `else`. |
| `c09_unary_chain` | Válido | Verificar varios operadores negativos unarios consecutivos. |
| `c10_full_precedence` | Válido | Verificar la combinación de los niveles de precedencia de operadores. |
| `c11_expression_eof` | Inválido | Detectar una expresión incompleta al llegar a `TOKEN_EOF`. |
| `c12_lexical_and_syntax` | Inválido | Verificar la continuación del análisis ante errores léxicos y sintácticos. |

El script `run_custom_tests.sh` ejecuta cada archivo y comprueba que el código de salida obtenido corresponda con el esperado para un programa válido o inválido.

## Pruebas de regresión del lexer

El directorio `lexer_public/` conserva las pruebas públicas utilizadas durante las Prácticas 1 y 2.

Se mantienen:

- 10 pruebas públicas heredadas de la Práctica 1;
- 13 pruebas públicas incorporadas en la Práctica 2;
- 23 pruebas públicas de regresión en total.

Para poder ejecutar estas pruebas con la nueva interfaz incremental del lexer se utiliza:

```text
tests/lexer_driver.c
```

Este programa solicita los tokens mediante la interfaz actual del lexer y los imprime con el formato utilizado en las prácticas anteriores. De esta forma se puede comprobar que la integración del parser no modificó el comportamiento léxico previamente desarrollado.

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

Los archivos se conservan como parte de las pruebas de regresión del lexer. El `Makefile` actual no incluye una regla específica para ejecutar automáticamente este conjunto.

## Resultado esperado

Las pruebas públicas y propias del parser deben clasificar correctamente cada archivo como válido o inválido.

Las pruebas públicas del parser también verifican el uso correcto de `stdout`, `stderr` y los códigos de salida.

Las pruebas públicas de regresión del lexer deben producir exactamente la misma secuencia de tokens esperada para las funcionalidades implementadas en las Prácticas 1 y 2.

La ejecución conjunta puede realizarse mediante:

```text
make test-all
```

y debe finalizar sin pruebas fallidas.