# Registro de cambios

Este archivo registra los cambios relevantes realizados durante el desarrollo incremental del compilador de MiniC.

## Práctica 3 — Analizador sintáctico descendente recursivo de MiniC

### Agregado

- Se agregó el módulo `parser` mediante los archivos `include/parser/parser.h` y `src/parser/parser.c`.
- Se incorporó la estructura `Parser` para mantener el lexer asociado, el token actual de anticipación, el estado de errores, el modo de recuperación y posibles fallos internos.
- Se implementó un analizador sintáctico descendente recursivo para validar programas escritos en MiniC.
- Se agregó el uso de un token de anticipación o *lookahead* mediante `parser->current`.
- Se implementaron las funciones auxiliares `parser_init`, `parser_advance`, `parser_check`, `parser_match`, `parser_current`, `parser_parse` y `parser_destroy`.
- Se implementó el reconocimiento de declaraciones de variables de tipo `int` y `bool`, con inicialización opcional.
- Se implementó el reconocimiento de asignaciones.
- Se implementó el reconocimiento de sentencias `print`.
- Se implementó el reconocimiento de sentencias condicionales `if` con rama `else` opcional.
- Se implementó la asociación del `else` con el `if` más cercano que todavía no tenga una rama `else`.
- Se implementó el reconocimiento de ciclos `while`.
- Se implementó el reconocimiento de bloques delimitados por `{` y `}` con cero o más sentencias.
- Se implementó el análisis de expresiones mediante funciones separadas por nivel de precedencia:
  - `parse_or_expression`
  - `parse_and_expression`
  - `parse_equality_expression`
  - `parse_comparison_expression`
  - `parse_additive_expression`
  - `parse_multiplicative_expression`
  - `parse_unary_expression`
  - `parse_primary`
- Se agregó soporte para los operadores `||`, `&&`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `+`, `-`, `*`, `/` y el operador unario `-`.
- Se agregó soporte para expresiones agrupadas mediante paréntesis.
- Se implementó `syntax_error` para reportar errores sintácticos con línea, columna, elemento esperado y lexema encontrado.
- Se implementó `parser_consume` para exigir tokens obligatorios durante el reconocimiento de producciones.
- Se implementó `synchronize` como estrategia básica de recuperación ante errores sintácticos.
- Se agregaron como puntos de sincronización `SEMICOLON`, `RBRACE`, `INT`, `BOOL`, `IF`, `ELSE`, `WHILE`, `PRINT`, `IDENTIFIER`, `LBRACE` y `TOKEN_EOF`.
- Se incorporó `panic_mode` para evitar diagnósticos repetidos durante la recuperación de un mismo error.
- Se agregó el manejo de tokens léxicos `ERROR` recibidos por el parser.
- Se incorporaron estados separados para fallos internos del lexer mediante `LEXER_STATUS_OK`, `LEXER_STATUS_IO_ERROR` y `LEXER_STATUS_MEMORY_ERROR`.
- Se agregaron las 17 pruebas públicas correspondientes a la Práctica 3.
- Se agregaron 12 pruebas propias para bloques vacíos, estructuras anidadas, `if` y `while` sin bloque, ausencia de validaciones semánticas, recuperación de múltiples errores, `else` sin `if`, llaves faltantes antes de `else`, cadenas de operadores unarios, precedencia completa, expresiones incompletas al final del archivo y combinación de errores léxicos y sintácticos.
- Se conservaron las pruebas públicas y propias del lexer de las prácticas anteriores como pruebas de regresión.
- Se agregó `tests/lexer_driver.c` para ejecutar las pruebas del lexer utilizando la nueva interfaz incremental.
- Se agregaron los objetivos `make test-custom`, `make test-lexer` y `make test-all`.

### Modificado

- Se reemplazó la interfaz `lexer_scan(FILE *file)`, que recorría completamente el archivo, por una interfaz incremental basada en `lexer_init`, `lexer_next_token` y `lexer_destroy`.
- Se modificó el lexer para producir exactamente un token por cada llamada a `lexer_next_token`.
- Se agregó la estructura `Lexer` para conservar entre llamadas el archivo fuente, la línea actual, la columna actual y el estado necesario para manejar correctamente secuencias `\r\n`.
- Se modificó el seguimiento de línea y columna para conservar su estado entre llamadas sucesivas al lexer.
- Se modificó el lexer para que solicitar un token no genere salida como efecto secundario.
- Se eliminó el uso de `token_print` durante la ejecución normal del programa y se conservó únicamente para las pruebas de regresión del lexer.
- Se modificó la propiedad de los tokens: el lexer crea y entrega el token, mientras que el parser adquiere la responsabilidad de liberar el lexema al avanzar o finalizar el análisis.
- Se modificó `parser_advance` para liberar el token actual antes de solicitar el siguiente.
- Se modificó el flujo de análisis para que los tokens `ERROR` sean reportados y consumidos antes de llegar a las funciones de reconocimiento sintáctico.
- Se modificó `src/main.c` para inicializar el lexer y el parser, ejecutar `parser_parse`, mostrar únicamente el resultado general y liberar los recursos utilizados.
- Se reservó `stdout` para el mensaje `Programa sintacticamente correcto.` y se enviaron los diagnósticos léxicos y sintácticos a `stderr`.
- Se establecieron códigos de salida distintos para programas válidos, programas con errores y fallos de uso, apertura del archivo o fallos internos.
- Se actualizó el `Makefile` para incluir `src/parser/parser.c` durante la compilación de `minic`.
- Se actualizó `make clean` para eliminar ejecutables y archivos generados durante la compilación y las pruebas.
- Se reorganizaron las pruebas heredadas del lexer dentro de `tests/lexer_public` y `tests/lexer_custom`.
- Se incorporaron las pruebas de la Práctica 3 dentro de `tests/public` y las pruebas propias dentro de `tests/parser_custom`.
- Se conservaron las funcionalidades léxicas y el sistema de tokens desarrollados en las Prácticas 1 y 2.
- Se mantuvo la compilación con las banderas `-std=c11 -Wall -Wextra -Wpedantic`.

### Corregido

- Se corrigió la conservación de la posición del lexer entre llamadas sucesivas a `lexer_next_token`.
- Se aseguró que comentarios, espacios en blanco y saltos de línea puedan consumirse sin producir tokens ni alterar incorrectamente la posición del siguiente token válido.
- Se evitó que un token `ERROR` produzca simultáneamente un diagnóstico léxico y uno sintáctico.
- Se aseguró que los fallos internos de lectura o memoria se distingan de los errores presentes en el programa fuente.
- Se corrigió el manejo de memoria de los tokens durante la transferencia de propiedad entre lexer y parser.
- Se eliminó una definición duplicada de `parse_primary` que impedía la compilación del parser.
- Se corrigió un error de enlazado provocado porque el `Makefile` inicialmente no incluía `src/parser/parser.c`.
- Se ajustó la implementación de expresiones para respetar la precedencia y asociatividad definidas por la gramática de MiniC.
- Se aseguró el reconocimiento recursivo del operador unario `-`.
- Se corrigió la recuperación ante errores para garantizar el avance del parser y evitar ciclos infinitos.
- Se ajustó la sincronización para consumir `SEMICOLON` cuando corresponde y conservar los tokens que pueden iniciar una nueva sentencia.
- Se aseguró que `RBRACE`, `ELSE` y `TOKEN_EOF` permanezcan disponibles cuando deben ser procesados por una estructura exterior.
- Se aseguró que un `ELSE` sin un `IF` asociado pueda ser reportado posteriormente como token inesperado.
- Se aseguró que el parser pueda continuar después de un error y detectar múltiples errores dentro del mismo archivo.
- Se aseguró el consumo completo de la entrada antes de considerar un programa sintácticamente correcto.


## Práctica 2 — Analizador léxico completo de MiniC

### Agregado

- Reconocimiento de identificadores mediante el patrón `[a-zA-Z_][a-zA-Z0-9_]*`.
- Funciones auxiliares `is_identifier_start` e `is_identifier_part` para distinguir el inicio y la continuación de un identificador.
- Clasificación de identificadores mediante `identifier_type` para reconocer las palabras reservadas `int`, `bool`, `if`, `else`, `while` y `print`.
- Reconocimiento de los literales booleanos `true` y `false` mediante los tokens `TRUE` y `FALSE`.
- Reconocimiento de los operadores compuestos `==`, `!=`, `<=`, `>=`, `&&` y `||`.
- Función auxiliar `compound_token_type` para clasificar operadores de dos caracteres.
- Anticipación de un carácter para distinguir entre operadores simples y compuestos.
- Detección de `!`, `&` y `|` aislados como tokens `ERROR`.
- Distinción entre el operador de división `/` y el inicio de comentarios `//`.
- Función `skip_line_comment` para consumir comentarios de una línea sin producir tokens.
- Actualización de línea y columna durante el procesamiento de comentarios.
- Integración de las 13 pruebas públicas proporcionadas para la Práctica 2.
- Incorporación de pruebas propias para casos límite de identificadores, secuencias de operadores, contenido de comentarios, transición entre números e identificadores, recuperación ante errores y comentarios terminados con `\r` aislado.

### Modificado

- Se extendió `lexer_scan` para reconocer identificadores antes de recurrir al manejo general de errores.
- Se modificó la clasificación de palabras reservadas para realizarse únicamente después de consumir el lexema completo del identificador.
- Se modificó la prioridad de reconocimiento para procesar operadores compuestos antes que sus variantes simples.
- Se modificó el tratamiento de `/` para reconocer comentarios `//` antes de generar un token `SLASH`.
- Se conservó la generación de un token `ERROR` por cada carácter no reconocido y la continuación del análisis desde el carácter siguiente.
- Se mantuvo el seguimiento de posición heredado de la Práctica 1 para espacios, `\n`, `\r` y `\r\n`, extendiéndolo al contenido de los comentarios.
- Se integraron las pruebas públicas de las Prácticas 1 y 2, junto con las pruebas propias, dentro de la regla `make test`.
- Se actualizó `tests/README.md` para documentar las funcionalidades y pruebas incorporadas en la Práctica 2.

### Corregido

- Se evitó reconocer parcialmente prefijos de palabras reservadas; por ejemplo, `while1` y `while_value` se conservan como `IDENTIFIER`.
- Se evitó dividir operadores compuestos en varios tokens simples; por ejemplo, `==` se reconoce como `EQUAL` y no como dos tokens `ASSIGN`.
- Se aseguró que `!`, `&` y `|` aislados produzcan `ERROR`, mientras que `!=`, `&&` y `||` sean reconocidos como operadores válidos.
- Se aseguró que el contenido de los comentarios no produzca tokens, incluso cuando contiene palabras reservadas, operadores o caracteres inválidos.
- Se mantuvo `/` como `SLASH` cuando no se encuentra seguido de otro `/`.
- Se ajustó la firma de `skip_line_comment` para utilizar `size_t *` en los parámetros correspondientes a línea y columna, de acuerdo con el seguimiento de posición existente.
- Se ajustó el uso de `advance_position` para mantener compatibilidad con su declaración y eliminar errores y advertencias de compilación.
- Se verificó el manejo correcto de comentarios terminados mediante `\n`, `\r`, `\r\n` o EOF.

## Práctica 1 — Infraestructura básica del analizador léxico

### Agregado

- Reconocimiento de los símbolos simples `-`, `*`, `/`, `=`, `<`, `>`, `(`, `)`, `{`, `}` y `;`.
- Reconocimiento de números enteros formados por uno o más dígitos.
- Crecimiento dinámico del lexema de los números enteros mediante `realloc`.
- Generación de un token `ERROR` por cada carácter no reconocido.
- Generación de `TOKEN_EOF` con la posición posterior al último carácter procesado.
- Soporte para saltos de línea `\n`, retornos de carro `\r` y secuencias `\r\n`.
- Incorporación de pruebas propias para varios caracteres no reconocidos, enteros largos, retornos de carro aislados y saltos de línea CRLF.
- Incorporación de un `Makefile` para compilar el proyecto, ejecutar las pruebas y eliminar los archivos generados.

### Modificado

- Se amplió `simple_token_type` para reconocer todos los símbolos simples requeridos en la Práctica 1.
- Se completó `lexer_scan` para reconocer números enteros, errores léxicos y el final del archivo.
- Se modificó el seguimiento de línea y columna para tratar `\r\n` como un único salto de línea.
- Se integraron las pruebas públicas y las pruebas propias dentro de la regla `make test`.

### Corregido

- Se corrigió la posición de `TOKEN_EOF` en archivos vacíos, archivos sin salto de línea final y archivos con espacios en blanco finales.
- Se evitó contar la secuencia `\r\n` como dos saltos de línea.
- Se sustituyó la salida provisional `PENDIENTE` por tokens `ERROR` con el formato establecido.
- Se corrigió el manejo de números negativos para producir un token `MINUS` seguido de un token `INTEGER`.