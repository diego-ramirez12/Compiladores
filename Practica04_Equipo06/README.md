# Práctica 4 — Árbol de Sintaxis Abstracta de MiniC

Este proyecto corresponde a la Práctica 4 del curso de Compiladores y extiende el
compilador desarrollado hasta la Práctica 3 para construir un Árbol de Sintaxis
Abstracta (AST) de los programas MiniC.

El lexer y el parser descendente recursivo desarrollados previamente se conservan.
En esta práctica, el parser además de validar la sintaxis construye nodos que
representan la estructura esencial del programa.

## Información general

| Campo | Información |
|---|---|
| Asignatura | Compiladores |
| Número de práctica | 4 |
| Semestre | 2027-1 |
| Equipo | 06 |

## Integrantes

| Nombre completo | Número de cuenta | Correo electrónico |
|---|---|---|
| Guzman Ramirez Diego Arturo | 321182324 | diego_aa@ciencias.unam.mx |
| Marcial Ahedo Andrick Iorel | 321304074 | marcialandrick@ciencias.unam.mx |

## Punto de partida

La Práctica 4 extiende el proyecto desarrollado en la Práctica 3.

Se conservaron:

- el lexer completo desarrollado en las prácticas anteriores;
- la interfaz incremental del lexer;
- el parser descendente recursivo;
- el token de anticipación;
- los diagnósticos léxicos y sintácticos;
- la estrategia de recuperación ante errores;
- la precedencia y asociatividad de las expresiones.

Sobre esta implementación se agregó el módulo `ast`, encargado de representar,
imprimir y liberar el Árbol de Sintaxis Abstracta.

En esta etapa no se realizan validaciones semánticas, construcción de tablas de
símbolos ni verificación de tipos.

## Contenido

La estructura principal del proyecto es:

```text
Practica04_Equipo06/
├── README.md
├── CHANGELOG.md
├── reporte.pdf
├── Makefile
├── include/
│   ├── lexer/
│   │   ├── lexer.h
│   │   └── token.h
│   ├── parser/
│   │   └── parser.h
│   └── ast/
│       └── ast.h
├── src/
│   ├── main.c
│   ├── lexer/
│   │   ├── lexer.c
│   │   └── token.c
│   ├── parser/
│   │   └── parser.c
│   └── ast/
│       └── ast.c
└── tests/
    ├── public/
    ├── ast_custom/
    ├── parser_custom/
    ├── lexer_public/
    └── lexer_custom/
```

Los módulos principales son:

- `include/lexer/` y `src/lexer/`: analizador léxico incremental de MiniC.
- `include/parser/` y `src/parser/`: analizador sintáctico descendente recursivo.
- `include/ast/` y `src/ast/`: definición, construcción, impresión y liberación
  del Árbol de Sintaxis Abstracta.
- `src/main.c`: integración del lexer, parser y AST.
- `tests/public/`: pruebas públicas correspondientes a la Práctica 4.
- `tests/ast_custom/`: pruebas propias para verificar la estructura del AST.
- `tests/parser_custom/`: pruebas de regresión del parser.
- `tests/lexer_public/`: pruebas de regresión del lexer.
- `tests/lexer_custom/`: pruebas adicionales del lexer desarrolladas anteriormente.

## AST

El AST representa únicamente la información esencial del programa.

Se implementaron nodos para:

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

La estructura del árbol conserva la precedencia y asociatividad definidas por la
gramática de MiniC.

Por ejemplo:

```text
x = 2 + 3 * 4;
```

produce una estructura donde la multiplicación es descendiente de la suma:

```text
Assignment(x)
  Add
    Integer(2)
    Multiply
      Integer(3)
      Integer(4)
```

Para:

```text
x = 10 - 5 - 2;
```

se conserva la asociatividad izquierda:

```text
Assignment(x)
  Subtract
    Subtract
      Integer(10)
      Integer(5)
    Integer(2)
```

Los paréntesis utilizados únicamente para agrupación no se representan como nodos.
Su efecto queda reflejado directamente en la estructura del árbol.

## Posición de los nodos

Cada nodo conserva la línea y columna iniciales de la construcción que representa.

La información de posición se almacena dentro del AST para poder utilizarse en
etapas posteriores del compilador.

Las posiciones no forman parte del formato canónico de impresión.

## Listas dinámicas

Los nodos `Program` y `Block` contienen una cantidad variable de sentencias.

Para almacenarlas se utiliza `ASTNodeList`, cuya capacidad aumenta dinámicamente
cuando es necesario.

No se utiliza un límite fijo para la cantidad de sentencias.

## Propiedad de memoria

El AST mantiene copias propias de los nombres de identificadores y de los lexemas
que necesita conservar.

Los nodos no almacenan apuntadores hacia cadenas temporales pertenecientes al
lexer o al parser.

La propiedad se maneja de la siguiente manera:

- cada nodo es propietario de sus nodos hijos;
- `Program` y `Block` son propietarios de sus listas de sentencias;
- la raíz es propietaria de todo el árbol;
- una lista adquiere la propiedad de un nodo únicamente cuando la inserción
  termina correctamente;
- un constructor adquiere la propiedad de sus hijos únicamente cuando termina
  correctamente.

La función:

```c
void ast_destroy(ASTNode *node);
```

libera recursivamente el árbol completo y acepta `NULL` sin realizar ninguna
operación.

También se liberan los árboles parciales cuando el análisis no puede completarse.

## Impresión

El AST utiliza el formato canónico especificado para la práctica.

Cada nivel utiliza exactamente dos espacios de indentación y se imprime un
elemento por línea.

Por ejemplo:

```text
AST:
Program
  VariableDeclaration(int, x)
    Integer(5)
  Assignment(x)
    Add
      Identifier(x)
      Integer(3)
  Print
    Identifier(x)
Programa sintacticamente correcto.
```

No se imprimen:

- tabuladores;
- conectores gráficos;
- direcciones de memoria;
- puntos y coma;
- paréntesis de agrupación;
- información de línea o columna;
- mensajes de depuración.

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

Para eliminar los archivos generados:

```text
make clean
```

## Ejecución

El programa recibe como argumento un archivo fuente MiniC:

```text
./minic programa.mc
```

Si el archivo completo es válido, se imprime el AST seguido de:

```text
Programa sintacticamente correcto.
```

Los diagnósticos se escriben exclusivamente en `stderr`.

Si se detecta algún error léxico o sintáctico:

- el AST no se imprime;
- no se imprime el mensaje de programa correcto;
- `stdout` permanece vacío;
- se libera cualquier árbol parcial;
- el proceso termina con un código distinto de cero.

Los códigos de salida utilizados son:

- `0`: programa válido;
- `1`: programa con errores léxicos o sintácticos;
- `2`: error de uso, apertura del archivo o fallo interno.

## Pruebas

Las pruebas públicas de la Práctica 4 pueden ejecutarse mediante:

```text
make test
```

o directamente:

```text
sh tests/public/run_public_tests.sh
```

Las pruebas propias del parser pueden ejecutarse mediante:

```text
make test-custom
```

Las pruebas de regresión del lexer pueden ejecutarse mediante:

```text
make test-lexer
```

Las pruebas propias del AST pueden ejecutarse mediante:

```text
make test-ast
```

Para ejecutar todos los conjuntos anteriores:

```text
make test-all
```

Los resultados actuales son:

```text
Pruebas públicas P4:      12/12
Pruebas propias parser:   12/12
Pruebas lexer:            23/23
Pruebas propias AST:       6/6
--------------------------------
Total:                    53/53
```

Las pruebas propias del AST verifican:

- asociatividad izquierda;
- precedencia completa;
- negaciones unarias encadenadas;
- bloques vacíos;
- condicionales anidados;
- ciclos anidados;
- formato exacto de impresión.

## Pruebas de memoria

La implementación fue revisada mediante Valgrind con programas válidos, programas
con errores sintácticos y programas con errores léxicos.

El comando utilizado es:

```text
valgrind --leak-check=full \
         --show-leak-kinds=all \
         ./minic programa.mc
```

En los casos probados se obtuvo:

```text
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

## Importante

- El lexer continúa funcionando de manera incremental y no imprime tokens durante
  la ejecución normal.
- El parser continúa siendo descendente recursivo.
- La recuperación de errores implementada en la Práctica 3 se conserva.
- Cada función principal del parser construye y devuelve el nodo correspondiente.
- Las cadenas necesarias se copian antes de liberar o reemplazar los tokens.
- Las listas de sentencias crecen dinámicamente.
- La precedencia y asociatividad quedan reflejadas en la estructura del AST.
- Cada nodo conserva su línea y columna.
- Un programa inválido no imprime un AST parcial.
- Liberar la raíz libera el árbol completo.
- No se realizan validaciones semánticas.
- No se construye una tabla de símbolos.
- No se verifican tipos.
- No se genera código.

Antes de generar el archivo ZIP de entrega se recomienda ejecutar:

```text
make clean
```

para evitar incluir ejecutables y archivos temporales generados durante la
compilación o las pruebas.