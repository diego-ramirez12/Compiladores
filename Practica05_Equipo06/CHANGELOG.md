# Registro de cambios

Este archivo registra los cambios relevantes realizados durante el desarrollo incremental del compilador de MiniC.

## Práctica 5 — Análisis semántico de MiniC

### Agregado

- Se agregó el módulo `semantic` mediante los archivos `include/semantic/` y `src/semantic/`.

- Se incorporaron los archivos `symbol.h`, `scope.h`, `symbol_table.h` y `semantic.h` para definir las estructuras e interfaces del análisis semántico.

- Se incorporaron los archivos `symbol.c`, `scope.c`, `symbol_table.c` y `semantic.c` para implementar las operaciones correspondientes.

- Se incorporó la estructura `Symbol` para representar las variables declaradas en los programas MiniC.

- Se agregó almacenamiento del nombre, tipo declarado, nivel de ámbito, línea, columna y referencia al nodo de declaración del AST.

- Se incorporó la enumeración `Type` para representar los tipos declarados de las variables.

- Se implementó `symbol_create` para crear símbolos y almacenar una copia independiente del nombre del identificador.

- Se implementó `symbol_destroy` para liberar la memoria perteneciente a un símbolo sin destruir su nodo de declaración del AST.

- Se incorporó la estructura `Scope` para representar ámbitos con una colección dinámica de símbolos, un nivel y una referencia al ámbito padre.

- Se implementó `scope_create` para crear el ámbito global y los ámbitos anidados.

- Se implementó `scope_destroy` para liberar los símbolos y recursos pertenecientes a un ámbito.

- Se incorporó la estructura `SymbolTable` para administrar el ámbito actual y los ámbitos creados durante el análisis.

- Se implementó `symbol_table_init` para inicializar la tabla de símbolos y crear el ámbito global.

- Se implementó `symbol_table_enter_scope` para crear un ámbito hijo y establecerlo como ámbito actual.

- Se implementó `symbol_table_exit_scope` para abandonar un ámbito y restaurar su ámbito padre sin permitir la salida del ámbito global.

- Se implementó `symbol_table_lookup_current` para buscar identificadores exclusivamente dentro del ámbito actual.

- Se implementó `symbol_table_lookup` para buscar identificadores desde el ámbito actual hasta el ámbito global.

- Se implementó `symbol_table_insert` para registrar símbolos y detectar declaraciones duplicadas dentro del mismo ámbito.

- Se agregó crecimiento dinámico de los arreglos de símbolos mediante `realloc`.

- Se incorporaron comprobaciones para evitar desbordamientos durante el crecimiento de los arreglos dinámicos de símbolos.

- Se implementó `symbol_table_destroy` para liberar todos los ámbitos y símbolos administrados por la tabla.

- Se agregó el manejo de ámbitos anidados mediante referencias al ámbito padre.

- Se implementó la visibilidad de identificadores declarados en ámbitos exteriores.

- Se agregó soporte para shadowing, permitiendo declaraciones con el mismo nombre en ámbitos diferentes.

- Se incorporó la conservación de ámbitos mediante `owned_scopes` y `next_owned`, permitiendo liberar sus recursos al destruir la tabla de símbolos.

- Se incorporó la estructura `SemanticAnalyzer` para administrar la tabla de símbolos, contabilizar errores semánticos y registrar fallos internos.

- Se implementó `semantic_analyzer_init` para inicializar el analizador semántico y su tabla de símbolos.

- Se implementó `semantic_analyze` para recorrer el AST y comprobar las reglas semánticas de MiniC.

- Se implementó `semantic_analyzer_destroy` para liberar los recursos administrados por el analizador semántico.

- Se implementó `semantic_report_internal_error` para reportar fallos internos del análisis semántico.

- Se agregaron funciones auxiliares para recorrer programas, sentencias, bloques, declaraciones, asignaciones y expresiones del AST.

- Se implementó el recorrido de las sentencias de un programa en el mismo orden en el que aparecen en el código fuente.

- Se agregó el registro de variables declaradas antes de recorrer sus expresiones inicializadoras.

- Se implementó la detección de identificadores utilizados sin una declaración visible.

- Se implementó la detección de redeclaraciones dentro del mismo ámbito.

- Se agregó la conservación de la primera declaración cuando se detecta una redeclaración.

- Se implementó la comprobación de identificadores utilizados como destino de asignaciones.

- Se agregó la búsqueda de identificadores utilizados dentro de expresiones, sentencias `print` y condiciones.

- Se incorporó el recorrido de expresiones binarias y unarias para verificar sus identificadores.

- Se incorporó el recorrido de sentencias `if` y `while`, incluyendo sus condiciones y cuerpos.

- Se agregó la creación y salida de ámbitos durante el recorrido de nodos `AST_BLOCK`.

- Se implementó la acumulación de múltiples errores semánticos dentro de un mismo programa.

- Se agregaron diagnósticos semánticos que incluyen el nombre del identificador, la línea y la columna del error.

- Se agregó información sobre la posición de la declaración original en los diagnósticos de redeclaración.

- Se incorporó la conversión entre los tipos `AST_TYPE_INT` y `AST_TYPE_BOOL` del AST y los tipos `TYPE_INT` y `TYPE_BOOL` de la tabla de símbolos.

- Se agregaron las 24 pruebas públicas correspondientes a la Práctica 5.

- Se agregaron cuatro pruebas unitarias para verificar individualmente los módulos `Symbol`, `Scope`, `SymbolTable` y `SemanticAnalyzer`.

- Se agregaron siete pruebas propias del análisis semántico para verificar shadowing en múltiples niveles, acumulación de errores, visibilidad, bloques hermanos, identificadores en expresiones y crecimiento dinámico.

- Se incorporó una prueba con 300 declaraciones para verificar el almacenamiento dinámico de símbolos.

- Se agregó el objetivo `make test-unit` para ejecutar las pruebas unitarias de los módulos semánticos.

- Se agregó el objetivo `make test-semantic-custom` para ejecutar las pruebas propias del análisis semántico.

### Modificado

- Se extendió el compilador desarrollado en la Práctica 4 para incorporar una etapa de análisis semántico posterior a la construcción del AST.

- Se conservó el lexer incremental desarrollado en las prácticas anteriores.

- Se conservó el parser descendente recursivo y su estrategia de recuperación ante errores léxicos y sintácticos.

- Se conservó la construcción del AST, incluyendo sus tipos de nodos, listas dinámicas, información de posición y liberación de memoria.

- Se modificó `src/main.c` para inicializar el analizador semántico después de obtener un AST válido.

- Se modificó `src/main.c` para ejecutar `semantic_analyze` únicamente cuando el programa supera correctamente el análisis léxico y sintáctico.

- Se modificó `src/main.c` para distinguir entre un programa válido, errores semánticos y fallos internos.

- Se modificó el flujo de ejecución para liberar la tabla de símbolos y el AST al finalizar el análisis.

- Se modificó la salida normal del compilador para imprimir únicamente `Programa semanticamente correcto.` cuando todas las validaciones terminan correctamente.

- Se eliminó la impresión automática del AST durante la ejecución normal, conservando la función `ast_print` para pruebas y depuración.

- Se sustituyó el mensaje `Programa sintacticamente correcto.` por el mensaje de éxito semántico correspondiente a la Práctica 5.

- Se mantuvo `stdout` vacío cuando existen errores léxicos, sintácticos o semánticos.

- Se conservaron los diagnósticos de errores exclusivamente en `stderr`.

- Se conservó el código de salida `0` para programas válidos y se utilizó `1` para programas con errores léxicos, sintácticos o semánticos.

- Se conservó el código de salida `2` para errores de uso, apertura del archivo o fallos internos.

- Se actualizó el `Makefile` para incluir `src/semantic/symbol.c`, `src/semantic/scope.c`, `src/semantic/symbol_table.c` y `src/semantic/semantic.c` durante la compilación del ejecutable `minic`.

- Se actualizaron las pruebas públicas dentro de `tests/public` para utilizar los casos correspondientes a la Práctica 5.

- Se conservaron las pruebas públicas de la Práctica 4 dentro de `tests/public_p4` como referencia.

- Se conservaron las pruebas públicas de la Práctica 3 dentro de `tests/public_p3` como referencia.

- Se conservaron las pruebas propias del AST y del parser desarrolladas en las prácticas anteriores.

- Se mantuvieron las pruebas públicas del lexer como pruebas de regresión mediante `tests/lexer_driver.c`.

- Se actualizó `make test` para ejecutar las pruebas públicas de la Práctica 5.

- Se actualizó `make test-all` para ejecutar las pruebas públicas de P05, las pruebas de regresión del lexer, las pruebas unitarias y las pruebas propias del análisis semántico.

- Se conservó la compilación con las banderas `-std=c11 -Wall -Wextra -Wpedantic`.

### Corregido

- Se aseguró que la búsqueda utilizada para detectar redeclaraciones considere únicamente los símbolos del ámbito actual.

- Se aseguró que la búsqueda visible comience en el ámbito más interno y continúe hacia el global.

- Se verificó que una variable declarada en un ámbito exterior pueda utilizarse dentro de sus ámbitos internos.

- Se aseguró que el shadowing permita declarar identificadores con el mismo nombre dentro de ámbitos diferentes.

- Se verificó que una declaración local oculte temporalmente una declaración exterior del mismo nombre.

- Se aseguró que, al abandonar un bloque, las variables pertenecientes a ese ámbito dejen de ser visibles.

- Se verificó que los símbolos de bloques hermanos no sean visibles entre sí.

- Se aseguró que una declaración posterior no valide retroactivamente un uso anterior del identificador.

- Se aseguró que una redeclaración no sustituya el símbolo correspondiente a la primera declaración.

- Se verificó que la inicialización de una variable se analice después de registrar su declaración en la tabla de símbolos.

- Se verificó que una declaración como `int x = x;` sea aceptada de acuerdo con las reglas establecidas para la Práctica 5.

- Se aseguró que los identificadores utilizados dentro de expresiones binarias y unarias también sean verificados.

- Se verificó que los identificadores utilizados en condiciones de `if` y `while` sean comprobados mediante la tabla de símbolos.

- Se aseguró que el recorrido semántico continúe después de encontrar errores recuperables para reportar múltiples problemas dentro del mismo programa.

- Se aseguró que los errores de reserva de memoria se distingan de los errores semánticos presentes en el programa fuente.

- Se aseguró que las operaciones fallidas de inserción no transfieran la propiedad del símbolo a la tabla.

- Se aseguró que la destrucción de símbolos no libere los nodos de declaración pertenecientes al AST.

- Se aseguró que los ámbitos abandonados permanezcan registrados para su liberación posterior, sin conservar su visibilidad.

- Se verificó que la destrucción de la tabla de símbolos libere los ámbitos, símbolos, nombres y arreglos dinámicos administrados por ella.

- Se aseguró que un programa con errores léxicos o sintácticos no inicie el análisis semántico.

- Se aseguró que un programa con errores semánticos no imprima el mensaje de éxito ni un AST parcial.

- Se verificó que el compilador no realice comprobaciones de compatibilidad de tipos durante la Práctica 5.

- Se verificó que el compilador no evalúe expresiones ni ejecute instrucciones de MiniC.

- Se verificó que los diagnósticos de los 12 casos públicos inválidos indiquen los identificadores, posiciones y causas esperados.

- Se verificó la implementación mediante 24 pruebas públicas de P05, 23 pruebas públicas de regresión del lexer, 4 pruebas unitarias y 7 pruebas propias del análisis semántico.

- Se obtuvo un total de 58 comprobaciones aprobadas y 0 fallidas mediante `make test-all`.

- Se verificó mediante Valgrind la liberación completa de memoria durante la ejecución de un programa válido con shadowing.

- Se verificó mediante Valgrind la liberación completa de memoria durante la ejecución de un programa con múltiples errores semánticos.

- Se verificó mediante Valgrind la ausencia de fugas de memoria y accesos inválidos en la prueba unitaria de la tabla de símbolos.


## Práctica 4 — Árbol de Sintaxis Abstracta de MiniC

### Agregado

- Se agregó el módulo `ast` mediante los archivos `include/ast/ast.h` y `src/ast/ast.c`.

- Se incorporó la estructura `ASTNode` para representar las construcciones sintácticas esenciales de los programas MiniC.

- Se agregaron los tipos de nodo `AST_PROGRAM`, `AST_BLOCK`, `AST_VARIABLE_DECLARATION`, `AST_ASSIGNMENT`, `AST_PRINT`, `AST_IF`, `AST_WHILE`, `AST_BINARY_EXPRESSION`, `AST_UNARY_EXPRESSION`, `AST_IDENTIFIER`, `AST_INTEGER_LITERAL` y `AST_BOOLEAN_LITERAL`.

- Se incorporó la enumeración `ASTDeclaredType` para distinguir declaraciones de tipo `int` y `bool`.

- Se incorporó la enumeración `BinaryOperator` para representar los operadores binarios `+`, `-`, `*`, `/`, `<`, `<=`, `>`, `>=`, `==`, `!=`, `&&` y `||`.

- Se incorporó la enumeración `UnaryOperator` para representar el operador negativo unario.

- Se agregó la estructura dinámica `ASTNodeList` para almacenar las sentencias pertenecientes a nodos `Program` y `Block`.

- Se implementó `ast_node_list_init` para inicializar listas de nodos.

- Se implementó `ast_node_list_append` para agregar nodos a una lista y aumentar dinámicamente su capacidad mediante `realloc`.

- Se implementó `ast_node_list_destroy` para liberar todos los nodos almacenados en una lista, liberar el arreglo interno y restablecer su estado.

- Se implementó una función auxiliar para copiar cadenas que deben sobrevivir a los tokens temporales producidos por el lexer.

- Se implementó `ast_create_program` para construir el nodo raíz de un programa.

- Se implementó `ast_create_block` para construir bloques con una lista dinámica de sentencias.

- Se implementó `ast_create_variable_declaration` para representar declaraciones de variables de tipo `int` o `bool`, con inicializador opcional.

- Se implementó `ast_create_assignment` para representar asignaciones y conservar el nombre de la variable destino.

- Se implementó `ast_create_print` para representar sentencias de impresión.

- Se implementó `ast_create_if` para representar sentencias condicionales con condición, rama verdadera y rama `else` opcional.

- Se implementó `ast_create_while` para representar ciclos con condición y cuerpo.

- Se implementó `ast_create_binary` para construir expresiones binarias con sus operandos izquierdo y derecho.

- Se implementó `ast_create_unary` para representar la negación unaria.

- Se implementó `ast_create_identifier` para representar identificadores y conservar una copia propia de su nombre.

- Se implementó `ast_create_integer` para representar literales enteros y conservar una copia propia de su lexema.

- Se implementó `ast_create_boolean` para representar los literales `true` y `false`.

- Se agregó almacenamiento de línea y columna en cada nodo del AST.

- Se implementó `ast_print` para recorrer e imprimir el árbol utilizando el formato canónico establecido para la práctica.

- Se agregó impresión con exactamente dos espacios por nivel de profundidad.

- Se agregaron las etiquetas canónicas `Program`, `Block`, `VariableDeclaration`, `Assignment`, `Print`, `If`, `Condition`, `Then`, `Else`, `While`, `Body`, `Add`, `Subtract`, `Multiply`, `Divide`, `Less`, `LessEqual`, `Greater`, `GreaterEqual`, `Equal`, `NotEqual`, `And`, `Or`, `Negate`, `Identifier`, `Integer` y `Boolean`.

- Se implementó `ast_destroy` para liberar recursivamente todos los nodos, cadenas, listas y arreglos pertenecientes al AST.

- Se agregó la construcción de nodos para expresiones primarias correspondientes a enteros, booleanos e identificadores.

- Se agregó la construcción recursiva de nodos `Negate` para expresiones unarias.

- Se agregó la construcción de nodos binarios en cada nivel de precedencia del parser.

- Se agregó la construcción del nodo raíz `Program` a partir de la lista ordenada de sentencias reconocidas.

- Se agregó la construcción de nodos `Block` para representar bloques vacíos o con una cantidad variable de sentencias.

- Se agregaron las 12 pruebas públicas correspondientes a la Práctica 4.

- Se agregaron seis pruebas propias del AST para verificar asociatividad izquierda, precedencia completa, operadores unarios encadenados, bloques vacíos, condicionales anidados y ciclos anidados.

- Se agregó el objetivo `make test-ast` para ejecutar las pruebas propias relacionadas con la estructura e impresión del AST.

- Se integraron las pruebas propias del AST al objetivo `make test-all`.

### Modificado

- Se modificó el parser descendente recursivo de la Práctica 3 para construir un Árbol de Sintaxis Abstracta mientras reconoce las producciones de MiniC.

- Se modificaron las funciones de reconocimiento de expresiones para devolver apuntadores `ASTNode *` en lugar de limitarse a indicar si una producción fue reconocida correctamente.

- Se modificó `parse_expression` para devolver la raíz de la expresión reconocida.

- Se modificó `parse_or_expression` para construir nodos `Or`.

- Se modificó `parse_and_expression` para construir nodos `And`.

- Se modificó `parse_equality_expression` para construir nodos `Equal` y `NotEqual`.

- Se modificó `parse_comparison_expression` para construir nodos `Less`, `LessEqual`, `Greater` y `GreaterEqual`.

- Se modificó `parse_additive_expression` para construir nodos `Add` y `Subtract`.

- Se modificó `parse_multiplicative_expression` para construir nodos `Multiply` y `Divide`.

- Se modificó `parse_unary_expression` para construir nodos `Negate` de forma recursiva.

- Se modificó `parse_primary` para construir nodos correspondientes a identificadores, enteros y booleanos.

- Se modificó el reconocimiento de expresiones agrupadas mediante paréntesis para devolver directamente el nodo correspondiente a la expresión interna, sin crear nodos adicionales para `(` o `)`.

- Se modificó `parse_declaration` para devolver un nodo de declaración y conservar el tipo, el nombre del identificador y el inicializador opcional.

- Se modificó `parse_assignment` para devolver un nodo de asignación con su expresión asociada.

- Se modificó `parse_print_statement` para devolver un nodo `Print`.

- Se modificó `parse_if_statement` para devolver un nodo `If` con condición, rama verdadera y rama falsa opcional.

- Se modificó `parse_while_statement` para devolver un nodo `While` con condición y cuerpo.

- Se modificó `parse_block` para construir una lista dinámica con las sentencias reconocidas dentro del bloque.

- Se modificó `parse_statement` para devolver el nodo correspondiente al tipo de sentencia reconocida.

- Se modificó `parser_parse` para construir y devolver el nodo raíz `Program`.

- Se modificó `parser_parse` para destruir la lista de nodos construidos cuando el programa contiene errores léxicos o sintácticos y evitar entregar un AST parcial.

- Se modificó la construcción de expresiones binarias para acumular el nodo izquierdo y conservar la asociatividad izquierda establecida por la gramática.

- Se conservó la separación de los niveles de precedencia implementada en la Práctica 3, utilizándola ahora para determinar la estructura jerárquica del AST.

- Se modificó la interfaz pública de `parser_parse` en `include/parser/parser.h` para devolver `ASTNode *`.

- Se agregó la dependencia del módulo `ast` a la interfaz del parser.

- Se modificó `src/main.c` para recibir la raíz del AST devuelta por el parser.

- Se modificó `src/main.c` para imprimir el encabezado `AST:` únicamente cuando el programa completo es válido.

- Se modificó `src/main.c` para invocar `ast_print` antes de imprimir `Programa sintacticamente correcto.`.

- Se modificó `src/main.c` para liberar el árbol mediante `ast_destroy` antes de terminar la ejecución.

- Se mantuvo `stdout` vacío cuando existe al menos un error léxico o sintáctico.

- Se conservaron los diagnósticos léxicos y sintácticos exclusivamente en `stderr`.

- Se conservó el código de salida `0` para programas válidos, `1` para programas con errores léxicos o sintácticos y `2` para errores de uso, apertura del archivo o fallos internos.

- Se actualizó el `Makefile` para incluir `src/ast/ast.c` durante la compilación del ejecutable `minic`.

- Se actualizaron las pruebas públicas dentro de `tests/public` para utilizar los casos y salidas esperadas de la Práctica 4.

- Se conservaron las pruebas propias del parser de la Práctica 3 para verificar que la incorporación del AST no modificara el reconocimiento sintáctico ni la recuperación ante errores.

- Se conservaron las pruebas públicas del lexer como pruebas de regresión.

- Se actualizó `make test-all` para ejecutar las pruebas públicas de la Práctica 4, las pruebas propias del parser, las pruebas de regresión del lexer y las pruebas propias del AST.

- Se conservó la compilación con las banderas `-std=c11 -Wall -Wextra -Wpedantic`.

### Corregido

- Se aseguró que los nombres de identificadores y los lexemas enteros almacenados en el AST sean copias independientes y no apuntadores hacia memoria temporal perteneciente al lexer o al parser.

- Se aseguró que cada nodo que adquiere hijos sea responsable de liberarlos cuando se destruye el árbol.

- Se aseguró que los nodos `Program` y `Block` sean propietarios de sus listas dinámicas de sentencias.

- Se aseguró que una operación fallida de `ast_node_list_append` no transfiera la propiedad del nodo a la lista.

- Se aseguró que los constructores de nodos únicamente adquieran la propiedad de sus hijos cuando terminan correctamente.

- Se aseguró que los nodos parcialmente construidos sean liberados cuando ocurre un error durante el análisis.

- Se aseguró que `ast_destroy` acepte `NULL` sin producir errores.

- Se aseguró que un programa con errores léxicos o sintácticos no imprima un AST parcial.

- Se aseguró que un programa inválido no imprima el mensaje `Programa sintacticamente correcto.`.

- Se aseguró que la impresión del AST respete exactamente dos espacios por nivel de profundidad.

- Se aseguró que las sentencias pertenecientes a `Program` y `Block` se impriman en el mismo orden en el que aparecen en el programa fuente.

- Se aseguró que las expresiones binarias impriman primero el operando izquierdo y después el operando derecho.

- Se aseguró que los paréntesis de agrupación modifiquen la estructura del árbol sin aparecer como nodos independientes.

- Se aseguró que los puntos y coma y demás elementos correspondientes únicamente a la sintaxis concreta no aparezcan en el AST.

- Se verificó que la expresión `10 - 5 - 2` produzca una estructura asociativa por la izquierda equivalente a `(10 - 5) - 2`.

- Se verificó que expresiones como `2 + 3 * 4` representen la multiplicación como descendiente de la suma.

- Se verificó que una cadena como `--x` produzca dos nodos `Negate` anidados.

- Se verificó la construcción correcta de bloques vacíos.

- Se verificó la construcción correcta de condicionales y ciclos anidados.

- Se verificó mediante las pruebas públicas que el formato canónico del AST coincida exactamente con las salidas esperadas.

- Se verificó mediante Valgrind la ausencia de fugas de memoria, accesos inválidos y liberaciones dobles en un programa válido.

- Se verificó mediante Valgrind la liberación completa de memoria después de un error sintáctico.

- Se verificó mediante Valgrind la liberación completa de memoria después de un error léxico.

- Se verificó la implementación mediante 12 pruebas públicas de la Práctica 4, 12 pruebas propias del parser, 23 pruebas públicas de regresión del lexer y 6 pruebas propias del AST, con un total de 53 pruebas aprobadas y 0 fallidas.

## Práctica 3 — Analizador sintáctico descendente recursivo de MiniC

### Agregado

- Se agregó el módulo `parser` mediante los archivos `include/parser/parser.h` y `src/parser/parser.c`.

- Se incorporó la estructura `Parser` para mantener el lexer asociado, el token actual de anticipación, el estado de errores, el modo de recuperación y posibles fallos internos.

- Se implementó un analizador sintáctico descendente recursivo para validar programas escritos en MiniC.

- Se agregó el uso de un token de anticipación o *lookahead* mediante `parser->current`.

- Se implementaron las funciones auxiliares `parser_init`, `parser_advance`, `parser_check`, `parser_match`, `parser_current`, `parser_parse` y `parser_destroy`.

- Se implementó el reconocimiento de declaraciones de variables de tipo `int` y `bool`, con inicialización opcional.

- Se implementó el reconocimiento de asignaciones mediante la producción `IDENTIFIER ASSIGN expression SEMICOLON`.

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

- Se agregó soporte para paréntesis dentro de expresiones.

- Se implementó `syntax_error` para reportar errores sintácticos con línea, columna, elemento esperado y lexema encontrado.

- Se implementó `parser_consume` para exigir tokens obligatorios durante el reconocimiento de producciones.

- Se implementó `synchronize` como estrategia básica de recuperación ante errores sintácticos.

- Se agregaron como puntos de sincronización `SEMICOLON`, `RBRACE`, `INT`, `BOOL`, `IF`, `ELSE`, `WHILE`, `PRINT`, `IDENTIFIER`, `LBRACE` y `TOKEN_EOF`.

- Se incorporó `panic_mode` para evitar diagnósticos repetidos durante la recuperación de un mismo error.

- Se agregó el manejo de errores léxicos recibidos por el parser, consumiendo los tokens `ERROR` después de reportarlos para evitar generar un segundo diagnóstico sintáctico sobre el mismo carácter.

- Se incorporaron estados separados para fallos internos del lexer mediante `LEXER_STATUS_OK`, `LEXER_STATUS_IO_ERROR` y `LEXER_STATUS_MEMORY_ERROR`.

- Se agregaron las 17 pruebas públicas correspondientes a la Práctica 3.

- Se agregaron 12 pruebas propias para bloques vacíos, estructuras anidadas, `if` y `while` sin bloque, ausencia de validaciones semánticas, recuperación de múltiples errores, `else` sin `if`, llaves faltantes antes de `else`, cadenas de operadores unarios, precedencia completa, expresiones incompletas al final del archivo y combinación de errores léxicos y sintácticos.

- Se conservaron las pruebas públicas y propias del lexer de la Práctica 2 como pruebas de regresión.

- Se agregó el objetivo `make test-custom` para ejecutar las pruebas propias del parser.

- Se agregó el objetivo `make test-lexer` para verificar que el lexer incremental conserve el comportamiento de la Práctica 2.

- Se agregó el objetivo `make test-all` para ejecutar las pruebas públicas del parser, las pruebas propias del parser y las pruebas de regresión del lexer.

### Modificado

- Se reemplazó la interfaz `lexer_scan(FILE *file)`, que recorría completamente el archivo, por una interfaz incremental basada en `lexer_init`, `lexer_next_token` y `lexer_destroy`.

- Se modificó el lexer para producir exactamente un token por cada llamada a `lexer_next_token`.

- Se agregó la estructura `Lexer` para conservar entre llamadas el archivo fuente, la línea actual, la columna actual y el estado necesario para manejar correctamente secuencias `\r\n`.

- Se modificó el manejo de línea y columna para conservar el estado entre llamadas sucesivas al lexer.

- Se modificó el lexer para que solicitar un token no genere salida como efecto secundario.

- Se eliminó el uso de `token_print` dentro de la ejecución normal del lexer.

- Se conservó `token_print` únicamente como utilidad para pruebas de regresión del analizador léxico.

- Se modificó la propiedad de los tokens: el lexer crea y entrega el token, mientras que el parser adquiere la responsabilidad de liberar el lexema asociado al avanzar al siguiente token o al finalizar el análisis.

- Se modificó `parser_advance` para liberar el token anterior antes de solicitar el siguiente token al lexer.

- Se modificó el flujo de análisis para que los tokens `ERROR` sean reportados y consumidos dentro de `parser_advance`, evitando que lleguen a las funciones de reconocimiento sintáctico.

- Se modificó `src/main.c` para inicializar el lexer y el parser, ejecutar `parser_parse`, mostrar únicamente el resultado general y liberar los recursos utilizados.

- Se reservó `stdout` exclusivamente para el mensaje `Programa sintacticamente correcto.`.

- Se enviaron los diagnósticos léxicos y sintácticos exclusivamente a `stderr`.

- Se estableció el código de salida `0` para programas válidos, `1` para programas con errores léxicos o sintácticos y un código distinto para errores de uso, apertura del archivo o fallos internos.

- Se actualizó el `Makefile` para incluir `src/parser/parser.c` durante la compilación del ejecutable `minic`.

- Se actualizó `make clean` para eliminar los ejecutables y archivos temporales generados durante las pruebas.

- Se reorganizaron las pruebas anteriores de la Práctica 2 dentro de `tests/lexer_public` y `tests/lexer_custom`.

- Se incorporaron las pruebas de la Práctica 3 dentro de `tests/public` y las pruebas propias dentro de `tests/parser_custom`.

- Se conservó la compilación con las banderas `-std=c11 -Wall -Wextra -Wpedantic`.

### Corregido

- Se corrigió la adaptación del lexer para conservar correctamente su posición entre llamadas sucesivas a `lexer_next_token`.

- Se aseguró que los comentarios, espacios en blanco y saltos de línea puedan consumirse sin producir tokens y sin perder la posición del siguiente token válido.

- Se evitó que un token `ERROR` generado por el lexer produzca simultáneamente un error léxico y un error sintáctico.

- Se aseguró que los fallos internos de lectura o memoria se distingan de los errores presentes en el programa fuente.

- Se corrigió el manejo de memoria de los tokens para evitar fugas y dobles liberaciones durante el cambio de propiedad entre lexer y parser.

- Se verificó mediante Valgrind que no existieran fugas de memoria ni accesos inválidos en programas válidos, programas con errores sintácticos y programas con errores léxicos.

- Se corrigió una definición duplicada de `parse_primary` que impedía la compilación del parser.

- Se corrigió un error de enlazado provocado porque el `Makefile` todavía no incluía `src/parser/parser.c`, lo que producía referencias indefinidas a `parser_init`, `parser_parse` y `parser_destroy`.

- Se ajustó la implementación de expresiones para respetar correctamente la precedencia y asociatividad definidas por la gramática de MiniC.

- Se aseguró que expresiones como `5 + 2 * 3` respeten la precedencia de multiplicación sobre suma sin necesidad de construir todavía un AST.

- Se aseguró que el operador unario `-` se reconozca de forma recursiva de acuerdo con la gramática.

- Se corrigió la recuperación ante errores para evitar permanecer indefinidamente en el mismo token.

- Se aseguró que `SEMICOLON` pueda consumirse durante la sincronización para continuar con la siguiente sentencia.

- Se aseguró que los tokens que pueden iniciar una nueva sentencia no sean consumidos durante la sincronización.

- Se aseguró que `RBRACE`, `ELSE` y `TOKEN_EOF` permanezcan disponibles para la estructura exterior correspondiente.

- Se aseguró que un `ELSE` sin un `IF` asociado sea reportado posteriormente como token inesperado.

- Se verificó que el parser pueda detectar múltiples errores dentro del mismo archivo sin detenerse después del primero.

- Se aseguró que el parser consuma toda la entrada y valide la presencia de `TOKEN_EOF`, evitando aceptar programas con contenido inesperado después de una sentencia válida.

- Se verificó que un archivo vacío sea aceptado como programa sintácticamente válido.

- Se verificó que el parser no realice validaciones semánticas, de tipos o de declaraciones previas, de acuerdo con el alcance de la práctica.

- Se verificó la implementación mediante 17 pruebas públicas del parser, 12 pruebas propias del parser y 23 pruebas públicas de regresión del lexer.

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