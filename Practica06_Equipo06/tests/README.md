# Pruebas

Este directorio contiene los casos utilizados para verificar el análisis semántico de MiniC de la Práctica 5, así como las pruebas unitarias de los nuevos módulos y las pruebas conservadas de las prácticas anteriores.

Las pruebas se encuentran organizadas de la siguiente forma:

```text
tests/
├── README.md
├── lexer_driver.c
├── symbol_unit.c
├── scope_unit.c
├── symbol_table_unit.c
├── semantic_unit.c
├── public/
│   ├── README.md
│   ├── cases.txt
│   ├── inputs/
│   ├── expected/
│   └── run_public_tests.sh
├── semantic_custom/
│   └── run_custom_tests.sh
├── lexer_public/
│   ├── README.md
│   ├── inputs/
│   ├── expected/
│   └── run_public_tests.sh
├── lexer_custom/
│   ├── inputs/
│   └── expected/
├── parser_custom/
│   ├── inputs/
│   └── run_custom_tests.sh
├── ast_custom/
│   ├── inputs/
│   ├── expected/
│   └── run_ast_tests.sh
├── public_p4/
│   ├── README.md
│   ├── inputs/
│   ├── expected/
│   └── run_public_tests.sh
└── public_p3/
    ├── README.md
    ├── inputs/
    ├── expected/
    └── run_public_tests.sh
```

## Ejecución

Los comandos deben ejecutarse desde la raíz del proyecto.

### Pruebas públicas de la Práctica 5

```text
make test
```

Ejecuta las 24 pruebas públicas correspondientes a la Práctica 5.

Estas pruebas verifican las reglas semánticas relacionadas con declaraciones, ámbitos, visibilidad, redeclaraciones y shadowing, además de conservar el manejo de errores léxicos y sintácticos.

### Pruebas propias del análisis semántico

```text
make test-semantic-custom
```

Ejecuta siete casos adicionales para comprobar el comportamiento del analizador semántico.

### Pruebas unitarias

```text
make test-unit
```

Ejecuta las pruebas individuales de los módulos `Symbol`, `Scope`, `SymbolTable` y `SemanticAnalyzer`.

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

1. las pruebas públicas de la Práctica 5;
2. las pruebas públicas de regresión del lexer;
3. las pruebas unitarias de los módulos semánticos;
4. las pruebas propias del análisis semántico.

Actualmente la ejecución conjunta verifica un total de 58 casos:

```text
Pruebas públicas P5:        24/24
Pruebas lexer:              23/23
Pruebas unitarias:           4/4
Pruebas propias semánticas:  7/7
--------------------------------
Total:                     58/58
```

## Pruebas públicas de la Práctica 5

El directorio `public/` contiene las 24 pruebas proporcionadas para la Práctica 5.

Estas pruebas verifican casos relacionados con:

- programas vacíos;
- declaraciones globales;
- uso válido de identificadores;
- uso de variables no declaradas;
- uso antes de declaración;
- redeclaraciones globales y locales;
- visibilidad en bloques anidados;
- uso de variables fuera de su ámbito;
- shadowing;
- bloques hermanos;
- inicializadores;
- múltiples errores semánticos;
- identificadores utilizados en expresiones y condiciones;
- sentencias `if` y `while`;
- ausencia de verificación de tipos;
- errores sintácticos y léxicos.

Para los programas válidos se comprueba:

- código de salida `0`;
- `stderr` vacío;
- coincidencia exacta de `stdout` con el mensaje esperado.

La salida válida es:

```text
Programa semanticamente correcto.
```

Para los programas inválidos se comprueba:

- código de salida distinto de cero;
- `stdout` vacío;
- presencia de diagnósticos en `stderr`.

Los nombres, posiciones y causas de los diagnósticos deben revisarse adicionalmente mediante:

```text
tests/public/expected/CASOS.md
```

La descripción de las pruebas y sus instrucciones se encuentran en:

```text
tests/public/README.md
```

## Pruebas propias del análisis semántico

El directorio `semantic_custom/` contiene siete pruebas adicionales desarrolladas para verificar el comportamiento del analizador semántico.

| Prueba | Resultado esperado | Propósito principal |
|---|---|---|
| `shadowing_multinivel` | Válido | Verificar shadowing en tres niveles de ámbito. |
| `errores_acumulados` | Inválido | Detectar múltiples errores semánticos dentro de un programa. |
| `uso_fuera_de_ambito` | Inválido | Detectar una variable utilizada después de abandonar su ámbito. |
| `bloques_hermanos` | Válido | Verificar declaraciones independientes en bloques hermanos. |
| `tipos_pospuestos_p06` | Válido | Confirmar que P05 no verifica compatibilidad de tipos. |
| `operandos_no_declarados` | Inválido | Detectar identificadores no declarados dentro de expresiones. |
| `trescientas_variables` | Válido | Verificar el crecimiento dinámico de la tabla de símbolos. |

El script:

```text
tests/semantic_custom/run_custom_tests.sh
```

genera los archivos de entrada temporales y comprueba:

- el código de salida correspondiente;
- el contenido de `stdout`;
- la presencia o ausencia de diagnósticos;
- la cantidad de errores esperados;
- los nombres y posiciones de los identificadores involucrados en los casos inválidos.

Las siete pruebas finalizaron correctamente.

## Pruebas unitarias

Se agregaron cuatro pruebas para verificar individualmente los módulos incorporados en la Práctica 5.

| Archivo | Propósito principal |
|---|---|
| `symbol_unit.c` | Verificar la creación, almacenamiento y destrucción de símbolos. |
| `scope_unit.c` | Verificar la creación de ámbitos, sus niveles y la liberación de recursos. |
| `symbol_table_unit.c` | Verificar inserción, búsquedas, redeclaraciones, shadowing y ámbitos anidados. |
| `semantic_unit.c` | Verificar el recorrido del AST y la detección de errores semánticos. |

Estas pruebas utilizan `assert` para comprobar los resultados esperados.

Se ejecutan mediante:

```text
make test-unit
```

Las cuatro pruebas finalizaron correctamente.

## Pruebas propias del AST

El directorio `ast_custom/` conserva las seis pruebas propias desarrolladas durante la Práctica 4.

Estas pruebas verifican:

- asociatividad izquierda;
- precedencia completa;
- operadores unarios encadenados;
- bloques vacíos;
- condicionales anidados;
- ciclos anidados.

Los archivos se conservan como referencia de la implementación del AST.

No forman parte de `make test-all`, ya que sus salidas esperadas corresponden al formato de impresión de la Práctica 4.

## Pruebas propias del parser

El directorio `parser_custom/` conserva los 12 casos adicionales desarrollados durante la Práctica 3.

Estas pruebas cubren:

- bloques vacíos;
- estructuras de control anidadas;
- sentencias `if` y `while` sin bloque;
- recuperación después de múltiples errores;
- `else` sin un `if` asociado;
- llaves faltantes;
- operadores unarios;
- precedencia de expresiones;
- expresiones incompletas;
- errores léxicos y sintácticos.

Las pruebas se conservan como referencia del comportamiento sintáctico desarrollado anteriormente.

No forman parte de `make test-all`, debido a que algunos programas aceptados sintácticamente pueden ser rechazados por las nuevas reglas semánticas de P05.

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

De esta forma se comprueba que la incorporación del parser, el AST y el analizador semántico no haya modificado el comportamiento léxico previamente desarrollado.

La descripción detallada se encuentra en:

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

Los archivos se conservan como referencia de las pruebas del lexer.

El `Makefile` actual no incluye una regla específica para ejecutar automáticamente este conjunto.

## Pruebas públicas de la Práctica 4

El directorio:

```text
tests/public_p4/
```

conserva las 12 pruebas públicas utilizadas durante la Práctica 4.

Estas pruebas verificaban la construcción e impresión del AST utilizando el formato canónico.

En P04, un programa válido producía una salida como:

```text
AST:
Program
  VariableDeclaration(int, x)
Programa sintacticamente correcto.
```

En P05, la salida normal es:

```text
Programa semanticamente correcto.
```

Por esta razón, las salidas esperadas de las pruebas públicas de P04 ya no coinciden con el contrato de salida actual.

El directorio se conserva únicamente como referencia y respaldo de la práctica anterior.

## Pruebas públicas de la Práctica 3

El directorio:

```text
tests/public_p3/
```

conserva las 17 pruebas públicas utilizadas durante la Práctica 3.

Estas pruebas verificaban el reconocimiento sintáctico de los programas MiniC.

En P03, un programa válido producía únicamente:

```text
Programa sintacticamente correcto.
```

En P05, el compilador también verifica las reglas semánticas y utiliza un mensaje de éxito diferente.

Por esta razón, las pruebas públicas originales de P03 no forman parte de `make test` ni de `make test-all`.

El directorio se mantiene como referencia y respaldo de la práctica anterior.

## Compatibilidad con prácticas anteriores

La Práctica 5 conserva las funcionalidades del lexer, parser y AST desarrolladas previamente.

Sin embargo, no todas las pruebas antiguas pueden ejecutarse sin modificaciones debido a los cambios en las validaciones y en el formato de salida.

La situación es la siguiente:

```text
Pruebas públicas P5       -> Sí, son las pruebas principales actuales.
Pruebas propias semánticas -> Sí.
Pruebas unitarias P5      -> Sí.
Pruebas públicas lexer    -> Sí, se utilizan como regresión.
Pruebas propias lexer     -> Se conservan como referencia.
Pruebas propias parser P3 -> Se conservan como referencia.
Pruebas propias AST P4    -> Se conservan como referencia.
Pruebas públicas P3       -> Se conservan como referencia.
Pruebas públicas P4       -> Se conservan como referencia.
```

Para verificar la implementación actual debe utilizarse:

```text
make test-all
```

## Pruebas de memoria

Además de las pruebas funcionales, la implementación fue verificada mediante Valgrind.

Se probaron:

- la tabla de símbolos mediante su prueba unitaria;
- un programa válido con shadowing;
- un programa con múltiples errores semánticos.

Por ejemplo:

```text
valgrind --leak-check=full \
         --show-leak-kinds=all \
         ./minic tests/public/inputs/p11_shadowing.mc
```

También se verificó:

```text
valgrind --leak-check=full \
         --show-leak-kinds=all \
         ./minic tests/public/inputs/p16_multiple_errors.mc
```

En ambos casos se obtuvo:

```text
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

Las ejecuciones realizadas confirmaron la liberación de los recursos del AST y de la tabla de símbolos en los casos probados.

## Resultado esperado

Las pruebas públicas de la Práctica 5 deben verificar que los programas válidos sean aceptados y que los inválidos produzcan los diagnósticos correspondientes.

Las pruebas propias del análisis semántico deben comprobar el comportamiento de los ámbitos, las declaraciones, la visibilidad y la recuperación ante errores.

Las pruebas unitarias deben verificar las operaciones de los módulos semánticos.

Las pruebas públicas de regresión del lexer deben producir exactamente la secuencia de tokens esperada para las funcionalidades implementadas en las Prácticas 1 y 2.

La ejecución conjunta puede realizarse mediante:

```text
make test-all
```

y debe finalizar sin pruebas fallidas.

El resultado actual es:

```text
Pruebas públicas P5:        24/24
Pruebas lexer:              23/23
Pruebas unitarias:           4/4
Pruebas propias semánticas:  7/7
--------------------------------
Total:                     58/58
```
