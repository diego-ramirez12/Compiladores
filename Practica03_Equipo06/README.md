# Práctica 3 — Analizador sintáctico descendente recursivo de MiniC

## Información general

| Campo | Información |
|---|---|
| Asignatura | Compiladores |
| Número de práctica | 3 |
| Semestre | 2027-1 |
| Equipo | 06 |

## Integrantes

| Nombre completo | Número de cuenta | Correo electrónico |
|---|---|---|
| Guzman Ramirez Diego Arturo | 321182324 | diego_aa@ciencias.unam.mx |
| Marcial Ahedo Andrick Iorel | 321304074 | marcialandrick@ciencias.unam.mx |

## Estructura del proyecto

```text
Practica03_Equipo06/
├── README.md
├── CHANGELOG.md
├── reporte.pdf
├── Makefile
├── include/
│   ├── lexer/
│   │   ├── lexer.h
│   │   └── token.h
│   └── parser/
│       └── parser.h
├── src/
│   ├── main.c
│   ├── lexer/
│   │   ├── lexer.c
│   │   └── token.c
│   └── parser/
│       └── parser.c
└── tests/
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

### Módulos implementados

| Archivo o módulo | Responsabilidad |
|---|---|
| `src/main.c` | Punto de entrada e integración. Valida los argumentos, abre el archivo fuente, inicializa el lexer y el parser, inicia el análisis, muestra el resultado general y libera los recursos utilizados. |
| `src/lexer/lexer.c` | Implementa el analizador léxico incremental heredado de la Práctica 2. Reconoce los tokens de MiniC y entrega un token por llamada al parser. |
| `src/lexer/token.c` | Implementa la creación, impresión y liberación de tokens y sus lexemas. |
| `include/lexer/lexer.h` | Define la estructura `Lexer`, los estados del analizador léxico y su interfaz incremental. |
| `include/lexer/token.h` | Define los tipos de token, la estructura `Token` y las operaciones asociadas. |
| `src/parser/parser.c` | Implementa el parser descendente recursivo, el reconocimiento de sentencias y expresiones, el manejo del lookahead, los diagnósticos y la recuperación ante errores. |
| `include/parser/parser.h` | Define la estructura `Parser` y la interfaz pública del analizador sintáctico. |
| `tests/` | Contiene las pruebas de la Práctica 3 y las pruebas de regresión del lexer desarrollado en las prácticas anteriores. |

## Requisitos

- GCC con soporte para C11.
- GNU Make.
- Dependencias adicionales: ninguna.
- Bash para ejecutar los scripts de pruebas.
- La compilación utiliza las opciones `-std=c11 -Wall -Wextra -Wpedantic`.

## Compilación

Desde la raíz del proyecto ejecutar:

```text
make
```

La compilación genera el ejecutable:

```text
minic
```

Para eliminar los archivos generados:

```text
make clean
```

## Ejecución

El programa recibe como argumento la ruta de un archivo fuente MiniC:

```text
./minic programa.mc
```

Si el programa no contiene errores léxicos ni sintácticos, la salida en `stdout` es:

```text
Programa sintacticamente correcto.
```

Los diagnósticos léxicos y sintácticos se envían a `stderr`.

Durante la ejecución normal no se imprime la secuencia de tokens.

Los códigos de salida utilizados son:

- `0`: programa sintácticamente correcto.
- `1`: programa con errores léxicos o sintácticos.
- `2`: error de uso, apertura del archivo o fallo interno.

## Funcionalidades implementadas

- [x] Reutilización del lexer desarrollado en la Práctica 2.
- [x] Interfaz incremental del lexer mediante `lexer_next_token`.
- [x] Seguimiento de línea y columna.
- [x] Manejo dinámico de tokens y lexemas.
- [x] Token de anticipación o lookahead.
- [x] Parser descendente recursivo.
- [x] Reconocimiento de declaraciones.
- [x] Reconocimiento de asignaciones.
- [x] Reconocimiento de sentencias `print`.
- [x] Reconocimiento de condicionales `if` y `else`.
- [x] Asociación del `else` con el `if` más cercano.
- [x] Reconocimiento de ciclos `while`.
- [x] Reconocimiento de bloques.
- [x] Expresiones con precedencia y asociatividad.
- [x] Operador negativo unario.
- [x] Expresiones agrupadas mediante paréntesis.
- [x] Detección de errores sintácticos.
- [x] Manejo de tokens `ERROR` provenientes del lexer.
- [x] Diagnósticos con línea y columna.
- [x] Recuperación ante errores mediante sincronización.
- [x] Continuación del análisis después de errores locales.
- [x] Verificación del archivo completo hasta `TOKEN_EOF`.
- [x] Liberación de los tokens utilizados por el parser.

En esta práctica el parser únicamente valida la sintaxis. No se construye un Árbol de Sintaxis Abstracta ni se realizan comprobaciones semánticas o de tipos.

## Pruebas

Las pruebas se encuentran dentro del directorio `tests/`.

### Pruebas públicas del parser

Para ejecutar las pruebas públicas correspondientes a la Práctica 3:

```text
make test
```

Estas pruebas verifican programas válidos e inválidos, declaraciones, asignaciones, sentencias `print`, precedencia de expresiones, condicionales, ciclos, bloques, recuperación ante errores, errores léxicos y consumo completo de la entrada.

### Pruebas propias del parser

Para ejecutar las pruebas adicionales desarrolladas por el equipo:

```text
make test-custom
```

Se incluyen 12 casos propios que comprueban, entre otros:

- bloques vacíos;
- estructuras de control anidadas;
- `if` y `while` sin bloques;
- ausencia de comprobaciones semánticas;
- recuperación ante múltiples errores;
- `else` aislado;
- ausencia de una llave antes de `else`;
- cadenas de operadores unarios;
- combinación completa de niveles de precedencia;
- expresiones incompletas al llegar a `TOKEN_EOF`;
- combinación de errores léxicos y sintácticos.

### Pruebas de regresión del lexer

Para ejecutar las pruebas públicas del lexer heredadas de las Prácticas 1 y 2:

```text
make test-lexer
```

Estas pruebas utilizan `tests/lexer_driver.c` para consumir la nueva interfaz incremental del lexer y comprobar que el comportamiento léxico anterior se conserve.

También se mantienen en `tests/lexer_custom/` las diez pruebas propias creadas durante las Prácticas 1 y 2.

### Ejecución conjunta

Para ejecutar las pruebas públicas del parser, las pruebas propias del parser y las pruebas públicas de regresión del lexer:

```text
make test-all
```

## Problemas conocidos

> No se conocen problemas adicionales a las limitaciones propias del alcance de la Práctica 3.

La implementación realiza únicamente validación sintáctica. La construcción del Árbol de Sintaxis Abstracta y las comprobaciones semánticas corresponden a prácticas posteriores.

## Notas de ejecución

Los comandos de compilación y prueba deben ejecutarse desde la raíz del proyecto, donde se encuentra el archivo `Makefile`.

Los comandos disponibles son:

```text
make
make clean
make test
make test-custom
make test-lexer
make test-all
```

Antes de generar el archivo ZIP de entrega se recomienda ejecutar:

```text
make clean
```

Esto elimina el ejecutable `minic`, `lexer_test` y otros archivos generados durante la compilación y las pruebas.