TaskScript - Analizador Léxico y Sintáctico
**Leguajes Formales y de Programación - USAC**  
**Proyecto 2 - 2026**

Descripción
TaskScript es una herramienta de escritorio desarrollada en C++ con Qt que permite 
analizar léxica y sintácticamente archivos `.task`, un mini-lenguaje para gestión 
de tareas tipo Kanban. Genera reportes HTML y un árbol de derivación en Graphviz.

## Compilación paso a paso
1. Abrir Qt Creator
2. File → Open File or Project
3. Seleccionar `CMakeLists.txt`
4. Seleccionar kit: **Desktop Qt 6.x MinGW 64-bit**
5. Click en **Build → Build Project** (Ctrl+B)
6. Click en **Run** (Ctrl+R)


## Uso de la aplicación
1. Ejecutar `TaskScript.exe`
2. Click en **Cargar Archivo .task**
3. Seleccionar un archivo `.task`
4. Click en **Analizar**
5. Ver tabla de tokens y errores en la interfaz
6. Abrir reportes con los botones: **Tablero Kanban**, **Carga Responsable**, **Tokens y Errores**
7. El archivo `arbol.dot` se genera en `C:/Users/[usuario]/TaskScript_Reportes/`

## Visualizar el árbol de derivación
```bash
dot -Tpng arbol.dot -o arbol.png

Alejandro Emmanuel Alvarez Velasquez
Carnet: 202404224
Universidad San Carlos de Guatemala
Facultad de Ingeniería - Ingeniería en Ciencias y Sistemas  
