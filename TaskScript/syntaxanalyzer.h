#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#include "token.h"
#include "errormanager.h"
#include <vector>
#include <string>

// Nodo del arbol de derivacion
struct NodoArbol {
    std::string etiqueta;
    bool esTerminal;
    std::vector<NodoArbol*> hijos;

    NodoArbol(std::string et, bool terminal = false)
        : etiqueta(et), esTerminal(terminal) {}

    ~NodoArbol() {
        for (auto h : hijos) delete h;
    }
};

// Estructura para almacenar los datos del tablero
struct Atributo {
    std::string prioridad;
    std::string responsable;
    std::string fechaLimite;
};

struct Tarea {
    std::string nombre;
    Atributo atributos;
};

struct Columna {
    std::string nombre;
    std::vector<Tarea> tareas;
};

struct Tablero {
    std::string nombre;
    std::vector<Columna> columnas;
};

class SyntaxAnalyzer {
public:
    SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& errorManager);

    bool analizar();
    NodoArbol* getArbol() const;
    Tablero getTablero() const;

private:
    std::vector<Token> tokens;
    int pos;
    ErrorManager& errorManager;
    NodoArbol* raiz;
    Tablero tablero;

    Token currentToken();
    Token consume(TokenType expected, const std::string& descripcion);
    void advance();
    bool check(TokenType type);
    bool match(TokenType type);

    // Funciones de cada produccion de la gramatica
    NodoArbol* parsePrograma();
    NodoArbol* parseColumnas();
    NodoArbol* parseColumna(Columna& columna);
    NodoArbol* parseTareas(std::vector<Tarea>& tareas);
    NodoArbol* parseTarea(Tarea& tarea);
    NodoArbol* parseAtributos(Atributo& atributo);
    NodoArbol* parseAtributo(Atributo& atributo);
    NodoArbol* parsePrioridad(std::string& prioridad);
};

#endif // SYNTAXANALYZER_H