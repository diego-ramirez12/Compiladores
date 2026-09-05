# Registro de cambios

Cambios relevantes realizados durante el desarrollo de la Práctica 1 se registran en este archivo.


### Agregado

* Reconocimiento de los símbolos simples "-", "*", "/", "=", "<", ">", "(", ")", "{", "}" y ";".
* Reconocimiento de números enteros formados por uno o más dígitos.
* Crecimiento del lexema de los números enteros mediante "realloc".
* Generación de un token "ERROR" por cada carácter no reconocido.
* Generación de "TOKEN_EOF" con la posición posterior al último carácter procesado.
* Soporte para saltos de línea "\n", retornos de carro "\r" y secuencias "\r\n".
* Pruebas propias para varios caracteres no reconocidos, enteros largos, retornos de carro aislados y saltos de línea CRLF.
* "Makefile" para compilar, ejecutar las pruebas y eliminar los archivos generados.

### Modificado

* Se amplió "simple_token_type" para reconocer todos los símbolos simples requeridos en la Práctica 1.
* Se completó "lexer_scan" para reconocer enteros, errores léxicos y el final del archivo.
* Se modificó el seguimiento de línea y columna para tratar "\r\n" como un único salto de línea.
* Se integraron las pruebas públicas y las pruebas propias en la regla "make test".

### Corregido

* Se corrigió la posición de "TOKEN_EOF" en archivos vacíos, archivos sin salto de línea final y archivos con espacios en blanco finales.
* Se evitó contar la secuencia "\r\n" como dos saltos de línea.
* Se sustituyó la salida provisional "PENDIENTE" por tokens "ERROR" con el formato establecido.
* Se corrigió el manejo de números negativos para producir un token "MINUS" seguido de un token "INTEGER".
