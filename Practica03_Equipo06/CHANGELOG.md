# Registro de cambios

Este archivo registra los cambios relevantes realizados durante el desarrollo incremental del analizador léxico de MiniC.

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
- Incorporación de seis pruebas propias para casos límite de identificadores, secuencias de operadores, contenido de comentarios, transición entre números e identificadores, recuperación ante errores y comentarios terminados con `\r` aislado.

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