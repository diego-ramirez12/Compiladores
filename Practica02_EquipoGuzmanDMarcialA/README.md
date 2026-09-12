# Práctica 2 — Analizador léxico completo de MiniC

## Información general

| Campo | Información |
|---|---|
| Asignatura | Compiladores |
| Número de práctica | 2 |
| Semestre | 2027-1 |
| Equipo | 6 |


## Integrantes

| Nombre completo | Número de cuenta | Correo electrónico |
|---|---|---|
| Guzman Ramirez Diego Arturo | 321182324 | diego_aa@ciencias.unam.mx |
| Marcial Ahedo Andrick Iorel | 321304074 | marcialandrick@ciencias.unam.mx |


## Estructura del proyecto

```text
Practica02_EquipoGuzmanDMarcialA/
├── README.md
├── CHANGELOG.md
├── reporte.pdf
├── Makefile
├── src/
│   ├── main.c
│   └── lexer/
│       ├── lexer.c
│       └── token.c
├── include/
│   └── lexer/
│       ├── lexer.h
│       └── token.h
└── tests/
    ├── README.md
    ├── public/
    │   ├── README.md
    │   ├── inputs/
    │   ├── expected/
    │   └── run_public_tests.sh
    └── custom/
        ├── inputs/
        └── expected/
```

### Módulos implementados

| Archivo o módulo | Responsabilidad |
|---|---|
| `src/main.c` | Punto de entrada del programa. Valida los argumentos, abre el archivo fuente, ejecuta el analizador léxico y cierra el archivo. |
| `src/lexer/lexer.c` | Contiene la lógica principal del analizador léxico. Reconoce números enteros, símbolos simples, identificadores, palabras reservadas, literales booleanos, operadores compuestos y comentarios de una sola línea. También realiza el seguimiento de línea y columna, genera tokens `ERROR`, permite la recuperación ante errores y produce `TOKEN_EOF`. |
| `src/lexer/token.c` | Implementa la creación, impresión y liberación de los tokens. Conserva el tipo, lexema, línea y columna inicial de cada token. |
| `include/lexer/lexer.h` | Declara las funciones utilizadas por el analizador léxico. |
| `include/lexer/token.h` | Define las categorías léxicas mediante `TokenType`, la estructura `Token` y las operaciones asociadas a los tokens. |
| `tests/` | Contiene las pruebas públicas proporcionadas para la práctica y las pruebas adicionales desarrolladas por el equipo. |

## Requisitos

- GCC con soporte para C11.
- GNU Make.
- Dependencias adicionales: ninguna.
- La compilación utiliza como mínimo las opciones `-std=c11 -Wall -Wextra -Wpedantic`.

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

El programa recibe como argumento la ruta de un archivo fuente:

```text
./minic programa.mc
```

Por ejemplo, para una entrada con el contenido:

```text
int contador = 10;
```

la salida es:

```text
1:0 INT int
1:4 IDENTIFIER contador
1:13 ASSIGN =
1:15 INTEGER 10
1:17 SEMICOLON ;
1:18 TOKEN_EOF
```

La salida se imprime en la salida estándar utilizando el formato establecido para los tokens:

```text
línea:columna TIPO_TOKEN lexema
```

Para `TOKEN_EOF` no se imprime lexema:

```text
línea:columna TOKEN_EOF
```

## Funcionalidades implementadas

- [x] Lectura del archivo fuente.
- [x] Sistema de tokens.
- [x] Seguimiento de línea y columna.
- [x] Manejo de espacios en blanco.
- [x] Manejo de `\n`, `\r` y `\r\n`.
- [x] Símbolos simples.
- [x] Números enteros.
- [x] Identificadores.
- [x] Palabras reservadas.
- [x] Literales booleanos.
- [x] Operadores compuestos.
- [x] Comentarios de una sola línea.
- [x] Caracteres no reconocidos mediante `ERROR`.
- [x] Recuperación después de errores léxicos.
- [x] Final del archivo mediante `TOKEN_EOF`.
- [x] Administración dinámica de lexemas.

Los identificadores se reconocen mediante el patrón:

```text
[a-zA-Z_][a-zA-Z0-9_]*
```

Las palabras reservadas y literales booleanos reconocidos son:

| Lexema | Token |
|---|---|
| `int` | `INT` |
| `bool` | `BOOL` |
| `if` | `IF` |
| `else` | `ELSE` |
| `while` | `WHILE` |
| `print` | `PRINT` |
| `true` | `TRUE` |
| `false` | `FALSE` |

El reconocimiento distingue entre mayúsculas y minúsculas.

Los operadores compuestos reconocidos son:

| Lexema | Token |
|---|---|
| `==` | `EQUAL` |
| `!=` | `NOT_EQUAL` |
| `<=` | `LESS_EQUAL` |
| `>=` | `GREATER_EQUAL` |
| `&&` | `AND` |
| `||` | `OR` |

Los caracteres `!`, `&` y `|` aislados producen un token `ERROR`.

Los comentarios comienzan con `//` y terminan al encontrar `\n`, `\r`, `\r\n` o el final del archivo. No generan tokens, pero actualizan correctamente la línea y la columna.

El carácter `/` continúa produciendo `SLASH` cuando no está seguido por otro `/`.

## Pruebas

Las pruebas se encuentran dentro del directorio `tests/` y se dividen en dos conjuntos:

- `tests/public/`: pruebas públicas proporcionadas con el material de la práctica.
- `tests/custom/`: pruebas adicionales desarrolladas por el equipo.

Para compilar el proyecto y ejecutar todas las pruebas:

```text
make test
```

Actualmente se incluyen 23 pruebas públicas y 10 pruebas propias, para un total de 33 casos. Durante el desarrollo, todas finalizaron correctamente.

Para una descripción detallada de los casos de prueba consulte `tests/README.md` y `tests/public/README.md`.

## Problemas conocidos

> No se conocen problemas adicionales a las limitaciones propias del alcance de la Práctica 2.

La implementación corresponde al analizador léxico completo definido para MiniC y servirá como entrada para las etapas posteriores del compilador.

## Notas de ejecución

Los comandos `make`, `make clean` y `make test` deben ejecutarse desde la raíz del proyecto, donde se encuentra el archivo `Makefile`.

Antes de generar el ZIP de entrega debe ejecutarse:

```text
make clean
```

Esto evita incluir el ejecutable `minic` o archivos temporales en la entrega.
