
# Práctica 5 — Análisis semántico de MiniC

Este proyecto corresponde a la Práctica 5 del curso de Compiladores y extiende el compilador desarrollado hasta la Práctica 4 para incorporar la primera etapa del análisis semántico de MiniC.

El lexer, el parser descendente recursivo y el Árbol de Sintaxis Abstracta (AST) desarrollados previamente se conservan. En esta práctica se agrega un analizador semántico que utiliza una tabla de símbolos con ámbitos anidados para detectar variables no declaradas, redeclaraciones y errores de visibilidad, permitiendo el shadowing.

## Información general

| Campo | Información |
|---|---|
| Asignatura | Compiladores |
| Número de práctica | 5 |
| Semestre | 2027-1 |
| Equipo | 06 |

## Integrantes

| Nombre completo | Número de cuenta | Correo electrónico |
|---|---|---|
| Guzman Ramirez Diego Arturo | 321182324 | diego_aa@ciencias.unam.mx |
| Marcial Ahedo Andrick Iorel | 321304074 | marcialandrick@ciencias.unam.mx |

## Punto de partida

La Práctica 5 extiende el proyecto desarrollado en la Práctica 4.

Se conservaron:

- el lexer completo desarrollado en las prácticas anteriores;
- la interfaz incremental del lexer;
- el parser descendente recursivo;
- el token de anticipación;
- los diagnósticos léxicos y sintácticos;
- la estrategia de recuperación ante errores;
- la precedencia y asociatividad de las expresiones;
- la construcción y liberación del AST;
- los tipos de nodos y las listas dinámicas de sentencias;
- la información de línea y columna.

Sobre esta implementación se agregó el módulo `semantic`, encargado de recorrer el AST, registrar declaraciones y verificar la visibilidad de identificadores mediante una tabla de símbolos con ámbitos anidados.

## Contenido

La estructura principal del proyecto es:

```text
Practica05_Equipo06/
├── README.md
├── CHANGELOG.md
├── Makefile
├── include/
│   ├── lexer/
│   │   ├── lexer.h
│   │   └── token.h
│   ├── parser/
│   │   └── parser.h
│   ├── ast/
│   │   └── ast.h
│   └── semantic/
│       ├── symbol.h
│       ├── scope.h
│       ├── symbol_table.h
│       └── semantic.h
├── src/
│   ├── main.c
│   ├── lexer/
│   │   ├── lexer.c
│   │   └── token.c
│   ├── parser/
│   │   └── parser.c
│   ├── ast/
│   │   └── ast.c
│   └── semantic/
│       ├── symbol.c
│       ├── scope.c
│       ├── symbol_table.c
│       └── semantic.c
└── tests/
    ├── README.md
    ├── public/
    ├── semantic_custom/
    ├── public_p4/
    ├── public_p3/
    ├── ast_custom/
    ├── parser_custom/
    ├── lexer_public/
    ├── lexer_custom/
    ├── lexer_driver.c
    ├── symbol_unit.c
    ├── scope_unit.c
    ├── symbol_table_unit.c
    └── semantic_unit.c
```

Los módulos principales son:

- `include/lexer/` y `src/lexer/`: analizador léxico incremental de MiniC.
- `include/parser/` y `src/parser/`: analizador sintáctico descendente recursivo.
- `include/ast/` y `src/ast/`: definición, construcción, impresión y liberación del AST.
- `include/semantic/` y `src/semantic/`: representación de símbolos, administración de ámbitos, tabla de símbolos y análisis semántico.
- `src/main.c`: integración del lexer, parser, AST y analizador semántico.
- `tests/public/`: pruebas públicas correspondientes a la Práctica 5.
- `tests/semantic_custom/`: pruebas propias del análisis semántico.
- `tests/*_unit.c`: pruebas unitarias de los módulos semánticos.
- `tests/lexer_public/`: pruebas de regresión del lexer.
- `tests/public_p3/` y `tests/public_p4/`: pruebas públicas conservadas de las prácticas anteriores.

## AST

El AST desarrollado en la Práctica 4 se conserva como representación estructurada del programa MiniC.

Se mantienen los nodos para:

- programa;
- bloques;
- declaraciones de variables;
- declaraciones con inicialización;
- asignaciones;
- sentencias `print`;
- condicionales `if` y `else`;
- ciclos `while`;
- expresiones binarias;
- expresiones unarias;
- identificadores;
- literales enteros;
- literales booleanos.

Las expresiones binarias representan los operadores:

```text
+
-
*
/
<
<=
>
>=
==
!=
&&
||
```

También se representa el operador negativo unario `-`.

La estructura del árbol conserva la precedencia y asociatividad definidas por la gramática de MiniC.

En esta práctica, el analizador semántico recorre los nodos del AST para registrar declaraciones y verificar los usos de identificadores.

Por ejemplo:

```c
int x;
x = 5;
```

es válido porque el identificador `x` fue declarado antes de utilizarse.

En cambio:

```c
x = 5;
```

produce un error semántico porque no existe una declaración visible de `x`.

El AST continúa construyéndose mediante el parser y no se modifica innecesariamente durante el análisis semántico.

## Posición de los nodos

Cada nodo conserva la línea y columna iniciales de la construcción que representa.

La información de posición se almacena dentro del AST y se utiliza para generar diagnósticos semánticos precisos.

Las posiciones no forman parte de la salida normal del compilador.

Por ejemplo, cuando se detecta una redeclaración, el analizador utiliza la posición de la declaración actual y la información almacenada en el símbolo original para reportar el error.

## Listas dinámicas

Los nodos `Program` y `Block` conservan las listas dinámicas de sentencias implementadas en la Práctica 4.

Para almacenarlas se utiliza `ASTNodeList`, cuya capacidad aumenta dinámicamente cuando es necesario.

Adicionalmente, los ámbitos del analizador semántico almacenan sus símbolos mediante arreglos dinámicos.

La capacidad de estos arreglos aumenta mediante `realloc` cuando se requiere almacenar más símbolos.

No se utiliza un límite fijo pequeño para la cantidad de sentencias o declaraciones.

## Propiedad de memoria

El AST conserva copias propias de los nombres de identificadores y de los lexemas necesarios para representar el programa.

Los nodos no almacenan apuntadores hacia cadenas temporales pertenecientes al lexer o al parser.

La propiedad se maneja de la siguiente manera:

- cada nodo del AST es propietario de sus nodos hijos;
- `Program` y `Block` son propietarios de sus listas de sentencias;
- la raíz es propietaria de todo el árbol;
- cada símbolo almacena una copia propia del nombre del identificador;
- la tabla de símbolos administra los símbolos y ámbitos creados;
- las referencias desde los símbolos hacia las declaraciones del AST no transfieren su propiedad;
- los ámbitos abandonados dejan de ser visibles y sus recursos se liberan al destruir la tabla;
- el analizador semántico no libera los nodos pertenecientes al AST.

La función:

```c
void ast_destroy(ASTNode *node);
```

libera recursivamente el árbol completo y acepta `NULL` sin realizar ninguna operación.

Por su parte, los módulos semánticos proporcionan funciones para liberar símbolos, ámbitos y la tabla de símbolos.

También se liberan los recursos adquiridos cuando el análisis no puede completarse correctamente.

## Impresión

A partir de la Práctica 5, el compilador deja de imprimir el AST durante la ejecución normal.

Si el programa supera las validaciones léxicas, sintácticas y semánticas, se imprime únicamente:

```text
Programa semanticamente correcto.
```

No se imprimen:

- tokens;
- nodos del AST;
- tablas de símbolos;
- ámbitos;
- direcciones de memoria;
- mensajes de depuración;
- el mensaje de éxito sintáctico de la Práctica 4.

La función `ast_print()` se conserva para pruebas y depuración, pero no se utiliza en la ejecución normal.

Los errores se reportan exclusivamente mediante `stderr`.

## Compilación

Desde la raíz del proyecto ejecutar:

```text
make clean
make
```

Se genera el ejecutable:

```text
minic
```

La compilación utiliza:

```text
-std=c11 -Wall -Wextra -Wpedantic
```

El `Makefile` incluye los módulos del lexer, parser, AST y analizador semántico.

Para eliminar los archivos generados:

```text
make clean
```

## Ejecución

El programa recibe como argumento un archivo fuente MiniC:

```text
./minic programa.mc
```

Si el archivo completo es válido, se imprime:

```text
Programa semanticamente correcto.
```

El análisis semántico únicamente se ejecuta cuando el programa supera las validaciones léxicas y sintácticas.

Si se detecta algún error léxico, sintáctico o semántico:

- no se imprime el AST;
- no se imprime el mensaje de programa correcto;
- `stdout` permanece vacío;
- los diagnósticos se escriben en `stderr`;
- se liberan los recursos correspondientes;
- el proceso termina con un código distinto de cero.

Los errores semánticos incluyen la posición y el nombre del identificador involucrado.

El analizador intenta continuar después de encontrar errores semánticos para reportar múltiples problemas dentro del mismo programa.

Los códigos de salida utilizados son:

- `0`: programa válido;
- `1`: programa con errores léxicos, sintácticos o semánticos;
- `2`: error de uso, apertura del archivo o fallo interno.

## Pruebas

Las pruebas públicas de la Práctica 5 pueden ejecutarse mediante:

```text
make test
```

o directamente:

```text
sh tests/public/run_public_tests.sh ./minic
```

Las pruebas de regresión del lexer pueden ejecutarse mediante:

```text
make test-lexer
```

Las pruebas unitarias de los módulos semánticos pueden ejecutarse mediante:

```text
make test-unit
```

Las pruebas propias del análisis semántico pueden ejecutarse mediante:

```text
make test-semantic-custom
```

Para ejecutar todos los conjuntos anteriores:

```text
make test-all
```

Los resultados obtenidos son:

```text
Pruebas públicas P5:        24/24
Pruebas de regresión lexer: 23/23
Pruebas unitarias:           4/4
Pruebas propias semánticas:  7/7
--------------------------------
Total:                     58/58
```

Las pruebas públicas de P05 verifican:

- declaraciones y usos válidos de variables;
- variables no declaradas;
- uso antes de declaración;
- redeclaraciones globales y locales;
- visibilidad de identificadores;
- ámbitos anidados y bloques hermanos;
- shadowing;
- inicializadores;
- múltiples errores semánticos;
- identificadores en expresiones y condiciones;
- errores léxicos y sintácticos;
- formato de salida y códigos de terminación.

Los diagnósticos de los casos inválidos pueden contrastarse con:

```text
tests/public/expected/CASOS.md
```

Las pruebas propias del análisis semántico verifican:

- shadowing en múltiples niveles;
- acumulación de errores;
- uso de variables fuera de su ámbito;
- independencia entre bloques hermanos;
- ausencia de verificación de tipos en P05;
- identificadores no declarados dentro de expresiones;
- crecimiento dinámico con 300 variables.

Las pruebas unitarias verifican individualmente los módulos `Symbol`, `Scope`, `SymbolTable` y `SemanticAnalyzer`.

Las pruebas públicas de P03 y P04 se conservan como referencia, pero no forman parte de `make test-all`, debido a los cambios en la salida y las validaciones del compilador.

## Pruebas de memoria

La implementación fue revisada mediante Valgrind con pruebas unitarias, programas válidos y programas con errores semánticos.

El comando utilizado es:

```text
valgrind --leak-check=full \
         --show-leak-kinds=all \
         ./minic programa.mc
```

Entre los casos verificados se encuentran:

```text
tests/public/inputs/p11_shadowing.mc
tests/public/inputs/p16_multiple_errors.mc
```

En las ejecuciones realizadas se obtuvo:

```text
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

Para el caso de shadowing se registraron 92 reservas y 92 liberaciones.

Para el programa con múltiples errores semánticos se registraron 58 reservas y 58 liberaciones.

La tabla de símbolos también se verificó de manera independiente, obteniendo 61 reservas y 61 liberaciones sin errores de memoria.

Los resultados corresponden a los casos probados mediante Valgrind.

## Importante

- El lexer continúa funcionando de manera incremental.
- El parser continúa siendo descendente recursivo.
- La construcción y liberación del AST de P04 se conserva.
- El análisis semántico utiliza el AST producido por el parser.
- El analizador semántico no vuelve a solicitar tokens.
- Se implementó una tabla de símbolos con ámbitos anidados.
- Existe un ámbito global para las declaraciones del programa.
- Cada bloque crea un nuevo ámbito.
- Las variables deben declararse antes de utilizarse.
- Las redeclaraciones dentro del mismo ámbito generan errores.
- El shadowing entre ámbitos diferentes está permitido.
- Las búsquedas de identificadores comienzan en el ámbito actual.
- Los símbolos de ámbitos abandonados dejan de ser visibles.
- Las declaraciones originales no se reemplazan después de una redeclaración.
- Cada símbolo conserva su tipo declarado, línea, columna y referencia al AST.
- Las sentencias se recorren en orden.
- Los errores semánticos se acumulan cuando es posible.
- No se realizan verificaciones de compatibilidad de tipos.
- No se evalúan expresiones ni se ejecuta el programa MiniC.
- No se genera código.
- No se imprime el AST durante la ejecución normal.
- La salida exitosa es `Programa semanticamente correcto.`.
- La tabla de símbolos y el AST se liberan independientemente.

Antes de generar el archivo ZIP de entrega se recomienda ejecutar:

```text
make test-all
make clean
```

De esta manera se comprueba el funcionamiento del proyecto y se evita incluir ejecutables generados durante la compilación.
