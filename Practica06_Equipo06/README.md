# Práctica 6 — Verificación de tipos de MiniC

Este proyecto corresponde a la Práctica 6 del curso de Compiladores y extiende el compilador desarrollado hasta la Práctica 5 para incorporar la verificación de tipos de MiniC.

Se conservan el lexer, el parser descendente recursivo, el Árbol de Sintaxis Abstracta (AST) y la tabla de símbolos con ámbitos anidados. En esta práctica se amplía el analizador semántico para comprobar la compatibilidad de tipos en expresiones, inicializaciones, asignaciones y estructuras de control.

## Información general

| Campo | Información |
|---|---|
| Asignatura | Compiladores |
| Número de práctica | 6 |
| Semestre | 2027-1 |
| Equipo | 06 |

## Integrantes

| Nombre completo | Número de cuenta | Correo electrónico |
|---|---|---|
| Guzman Ramirez Diego Arturo | 321182324 | diego_aa@ciencias.unam.mx |
| Marcial Ahedo Andrick Iorel | 321304074 | marcialandrick@ciencias.unam.mx |

## Punto de partida

La Práctica 6 extiende el proyecto desarrollado en la Práctica 5.

Se conservaron:

- el lexer completo y su interfaz incremental;
- el parser descendente recursivo;
- los diagnósticos léxicos y sintácticos;
- la estrategia de recuperación ante errores;
- la construcción y liberación del AST;
- la precedencia y asociatividad de las expresiones;
- la tabla de símbolos;
- la administración de ámbitos anidados;
- la detección de variables no declaradas;
- la detección de redeclaraciones;
- las reglas de visibilidad y shadowing;
- la información de línea y columna.

Sobre esta implementación se agregó la verificación de tipos dentro del analizador semántico.

Ahora el compilador comprueba que las operaciones y asignaciones sean compatibles con los tipos `int` y `bool`.

## Contenido

La estructura principal del proyecto es:

```text
Practica06_Equipo06/
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
    ├── p06_public/
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

- `include/lexer/` y `src/lexer/`: analizador léxico incremental.
- `include/parser/` y `src/parser/`: analizador sintáctico descendente recursivo.
- `include/ast/` y `src/ast/`: construcción, impresión y liberación del AST.
- `include/semantic/` y `src/semantic/`: tabla de símbolos, ámbitos y verificación semántica de tipos.
- `src/main.c`: integración de los componentes del compilador.
- `tests/p06_public/`: pruebas públicas correspondientes a la Práctica 6.
- `tests/public/`: pruebas públicas heredadas de la Práctica 5.
- `tests/semantic_custom/`: pruebas propias del análisis semántico.
- `tests/*_unit.c`: pruebas unitarias de los módulos semánticos.
- `tests/lexer_public/`: pruebas de regresión del lexer.

## AST

Se conserva el Árbol de Sintaxis Abstracta desarrollado en las prácticas anteriores.

El AST representa:

- declaraciones de variables;
- inicializaciones;
- asignaciones;
- bloques;
- sentencias `print`;
- condicionales `if` y `else`;
- ciclos `while`;
- expresiones binarias y unarias;
- identificadores;
- literales enteros y booleanos.

La precedencia y asociatividad de las expresiones se mantienen.

En esta práctica, el analizador semántico recorre el AST y determina los tipos de las expresiones.

Los tipos se obtienen durante el recorrido mediante la función `visit_expression()`, que devuelve un valor de tipo `Type`.

No fue necesario agregar campos de tipos a los nodos del AST.

## Verificación de tipos

MiniC utiliza dos tipos principales:

- `int`: números enteros.
- `bool`: valores booleanos.

También se utilizan internamente:

- `TYPE_ERROR`: indica que una expresión contiene un error semántico.
- `TYPE_UNKNOWN`: representa un tipo que todavía no se ha determinado.

Las reglas implementadas son:

| Construcción | Regla |
|---|---|
| Literal entero | Produce `int` |
| Literal booleano | Produce `bool` |
| Identificador | Utiliza el tipo de su declaración visible |
| `+`, `-`, `*`, `/` | Requieren `int` y producen `int` |
| Negativo unario `-` | Requiere `int` y produce `int` |
| `<`, `<=`, `>`, `>=` | Requieren `int` y producen `bool` |
| `==`, `!=` | Requieren operandos del mismo tipo y producen `bool` |
| `&&`, `\|\|` | Requieren `bool` y producen `bool` |
| Inicialización | El tipo de la expresión debe coincidir con el declarado |
| Asignación | El tipo de la expresión debe coincidir con el de la variable |
| `if` y `while` | La condición debe ser `bool` |
| `print` | Acepta expresiones `int` y `bool` |

No se realizan conversiones implícitas entre `int` y `bool`.

Por ejemplo:

```c
int x = 5;
bool activo = true;

x = x + 2;

if (activo) {
    print(x);
}
```

es semánticamente correcto.

En cambio:

```c
int x = true;
x = false;
```

produce errores porque se intenta utilizar un valor booleano donde se requiere un entero.

Cuando una expresión contiene un error, se propaga `TYPE_ERROR` para evitar diagnósticos adicionales causados por el mismo problema.

El analizador continúa revisando las demás expresiones y sentencias para detectar errores independientes.

## Posición de los nodos

Cada nodo del AST conserva la línea y columna correspondientes a su posición en el programa.

Esta información se utiliza para reportar errores semánticos de tipos.

Los diagnósticos indican la ubicación, la operación involucrada y los tipos esperados y encontrados cuando corresponde.

## Listas dinámicas

Se conservan las listas dinámicas de sentencias implementadas en la Práctica 4.

Los nodos `Program` y `Block` utilizan `ASTNodeList` para almacenar sus sentencias.

Los ámbitos de la tabla de símbolos también utilizan arreglos dinámicos.

No se establece un límite fijo pequeño para la cantidad de declaraciones o sentencias.

## Propiedad de memoria

Se conserva el manejo de memoria desarrollado en las prácticas anteriores.

La propiedad de las estructuras se organiza de la siguiente manera:

- cada nodo del AST es propietario de sus hijos;
- la raíz es propietaria de todo el árbol;
- la tabla de símbolos administra sus símbolos y ámbitos;
- cada símbolo conserva una copia propia del nombre del identificador;
- las referencias de los símbolos hacia el AST no transfieren su propiedad;
- el analizador semántico no libera directamente los nodos del AST.

La función `ast_destroy()` libera recursivamente el árbol completo.

La tabla de símbolos se libera mediante sus funciones de destrucción.

También se ajustó la liberación de nodos parciales del parser cuando ocurre un fallo interno durante la construcción del AST.

## Impresión

El compilador no imprime tokens, el AST ni la tabla de símbolos durante la ejecución normal.

Si el programa es válido, la salida es únicamente:

```text
Programa semanticamente correcto.
```

Si se detecta un error léxico, sintáctico o semántico, los diagnósticos se envían a `stderr`.

No se imprime un mensaje de éxito cuando existen errores.

La función `ast_print()` se conserva para pruebas y depuración.

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

Si el archivo es válido, se imprime:

```text
Programa semanticamente correcto.
```

El análisis semántico únicamente se ejecuta cuando el programa supera las validaciones léxicas y sintácticas.

Si existen errores:

- `stdout` permanece vacío;
- los diagnósticos se escriben en `stderr`;
- no se imprime el mensaje de éxito;
- se liberan los recursos utilizados;
- el programa termina con un código distinto de cero.

Los códigos de salida utilizados son:

- `0`: programa válido;
- `1`: programa con errores léxicos, sintácticos o semánticos;
- `2`: error de uso, apertura del archivo o fallo interno.

## Pruebas

Las pruebas públicas de la Práctica 6 pueden ejecutarse mediante:

```text
make test-p06-public
```

Las pruebas heredadas de la Práctica 5 pueden ejecutarse mediante:

```text
make test
```

Las pruebas de regresión del lexer:

```text
make test-lexer
```

Las pruebas unitarias de los módulos semánticos:

```text
make test-unit
```

Las pruebas propias del análisis semántico:

```text
make test-semantic-custom
```

El comando:

```text
make test-all
```

incluye todos estos conjuntos de pruebas.

Los resultados obtenidos son:

```text
Pruebas públicas P06:        24/24
Pruebas públicas P05:        23/24
Pruebas de regresión lexer:  23/23
Pruebas unitarias:            4/4
Pruebas propias semánticas:   6/7
```

Las pruebas públicas de P06 verifican:

- inicializaciones y asignaciones válidas e inválidas;
- operadores aritméticos y negativo unario;
- operadores relacionales;
- operadores de igualdad;
- operadores lógicos;
- condiciones de `if` y `while`;
- ámbitos y shadowing;
- variables no declaradas;
- propagación de `TYPE_ERROR`;
- prevención de errores en cascada;
- detección de errores independientes;
- errores léxicos y sintácticos.

Los diagnósticos esperados se encuentran en:

```text
tests/p06_public/esperados/CASOS.md
```

### Compatibilidad con pruebas anteriores

Se conservaron las pruebas originales de la Práctica 5.

Dos casos presentan resultados diferentes debido a la incorporación de la verificación de tipos:

- `p20_types_deferred`, de las pruebas públicas de P05.
- `tipos_pospuestos_p06`, de las pruebas semánticas propias de P05.

Ambas pruebas consideran válidos programas con incompatibilidades de tipos, ya que en P05 todavía no se realizaba esta comprobación.

En P06 esos programas deben rechazarse, por lo que el compilador reporta correctamente los errores.

Se conservaron las pruebas sin modificar para mostrar el cambio de comportamiento entre prácticas.

Por esta razón, `make test` y `make test-semantic-custom` reportan un fallo esperado cada uno, y `make test-all` termina con un código distinto de cero.

Las pruebas específicas de P06 pueden ejecutarse de manera independiente con `make test-p06-public`.

## Pruebas de memoria

La implementación fue revisada mediante Valgrind.

El comando utilizado fue:

```text
valgrind --leak-check=full \
         --show-leak-kinds=all \
         ./minic tests/p06_public/casos/p22_completo_valido.mc
```

En esta ejecución se obtuvo:

```text
total heap usage: 120 allocs, 120 frees
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

No se detectaron fugas ni errores de memoria en el programa probado.

## Importante

- Se conservan las funcionalidades desarrolladas hasta la Práctica 5.
- El análisis de tipos se realiza recorriendo el AST.
- Se utiliza la tabla de símbolos para obtener los tipos declarados.
- Se respetan los ámbitos anidados y el shadowing.
- Las variables deben declararse antes de utilizarse.
- Se verifican inicializaciones y asignaciones.
- Se validan operadores aritméticos, relacionales, lógicos y de igualdad.
- Las condiciones de `if` y `while` deben ser booleanas.
- No existen conversiones implícitas entre `int` y `bool`.
- Se utiliza `TYPE_ERROR` para evitar errores en cascada.
- Se continúa el análisis para encontrar errores independientes.
- No se evalúan expresiones ni se ejecuta el programa MiniC.
- No se generan optimizaciones ni código.
- Los recursos del AST y de la tabla de símbolos se liberan independientemente.
- La salida exitosa es `Programa semanticamente correcto.`.

Antes de generar el archivo ZIP de entrega se recomienda ejecutar:

```text
make test-p06-public
make clean
```

Esto permite comprobar las pruebas de P06 y eliminar los ejecutables generados durante la compilación.