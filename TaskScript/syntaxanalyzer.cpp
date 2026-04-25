#include "syntaxanalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& em)
    : tokens(tokens), pos(0), errorManager(em), raiz(nullptr) {}

Token SyntaxAnalyzer::currentToken() {
    if (pos < (int)tokens.size()) return tokens[pos];
    return tokens.back();
}

void SyntaxAnalyzer::advance() {
    if (pos < (int)tokens.size() - 1) pos++;
}

bool SyntaxAnalyzer::check(TokenType type) {
    return currentToken().type == type;
}

bool SyntaxAnalyzer::match(TokenType type) {
    if (check(type)) { advance(); return true; }
    return false;
}

Token SyntaxAnalyzer::consume(TokenType expected, const std::string& descripcion) {
    if (check(expected)) {
        Token t = currentToken();
        advance();
        return t;
    }
    Token t = currentToken();
    errorManager.agregarError(t.lexema, ErrorType::SINTACTICO,
                              descripcion + ", se encontro: '" + t.lexema + "'",
                              t.linea, t.columna);
    return t;
}

// <programa> ::= TABLERO CADENA "{" <columnas> "}" ";"
NodoArbol* SyntaxAnalyzer::parsePrograma() {
    NodoArbol* nodo = new NodoArbol("<programa>");

    // TABLERO
    Token t = consume(TokenType::TABLERO, "Se esperaba 'TABLERO'");
    nodo->hijos.push_back(new NodoArbol("TABLERO", true));

    // CADENA (nombre del tablero)
    Token nombre = consume(TokenType::CADENA, "Se esperaba el nombre del tablero entre comillas");
    tablero.nombre = nombre.lexema;
    nodo->hijos.push_back(new NodoArbol("\"" + nombre.lexema + "\"", true));

    // {
    consume(TokenType::LLAVE_ABRE, "Se esperaba '{'");
    nodo->hijos.push_back(new NodoArbol("{", true));

    // <columnas>
    NodoArbol* columnas = parseColumnas();
    nodo->hijos.push_back(columnas);

    // }
    consume(TokenType::LLAVE_CIERRA, "Se esperaba '}'");
    nodo->hijos.push_back(new NodoArbol("}", true));

    // ;
    consume(TokenType::PUNTO_COMA, "Se esperaba ';'");
    nodo->hijos.push_back(new NodoArbol(";", true));

    return nodo;
}

// <columnas> ::= <columna> <columnas> | <columna>
NodoArbol* SyntaxAnalyzer::parseColumnas() {
    NodoArbol* nodo = new NodoArbol("<columnas>");

    while (check(TokenType::COLUMNA)) {
        Columna col;
        NodoArbol* columna = parseColumna(col);
        nodo->hijos.push_back(columna);
        tablero.columnas.push_back(col);
    }

    return nodo;
}

// <columna> ::= COLUMNA CADENA "{" <tareas> "}" ";"
NodoArbol* SyntaxAnalyzer::parseColumna(Columna& columna) {
    NodoArbol* nodo = new NodoArbol("<columna>");

    // COLUMNA
    consume(TokenType::COLUMNA, "Se esperaba 'COLUMNA'");
    nodo->hijos.push_back(new NodoArbol("COLUMNA", true));

    // CADENA (nombre columna)
    Token nombre = consume(TokenType::CADENA, "Se esperaba el nombre de la columna");
    columna.nombre = nombre.lexema;
    nodo->hijos.push_back(new NodoArbol("\"" + nombre.lexema + "\"", true));

    // {
    consume(TokenType::LLAVE_ABRE, "Se esperaba '{'");
    nodo->hijos.push_back(new NodoArbol("{", true));

    // <tareas>
    NodoArbol* tareas = parseTareas(columna.tareas);
    nodo->hijos.push_back(tareas);

    // }
    consume(TokenType::LLAVE_CIERRA, "Se esperaba '}'");
    nodo->hijos.push_back(new NodoArbol("}", true));

    // ;
    consume(TokenType::PUNTO_COMA, "Se esperaba ';'");
    nodo->hijos.push_back(new NodoArbol(";", true));

    return nodo;
}

// <tareas> ::= <tarea> "," <tareas> | <tarea>
NodoArbol* SyntaxAnalyzer::parseTareas(std::vector<Tarea>& tareas) {
    NodoArbol* nodo = new NodoArbol("<tareas>");

    while (check(TokenType::TAREA)) {
        Tarea tarea;
        NodoArbol* t = parseTarea(tarea);
        nodo->hijos.push_back(t);
        tareas.push_back(tarea);

        if (check(TokenType::COMA)) {
            nodo->hijos.push_back(new NodoArbol(",", true));
            advance();
        }
    }

    return nodo;
}

// <tarea> ::= tarea ":" CADENA "[" <atributos> "]"
NodoArbol* SyntaxAnalyzer::parseTarea(Tarea& tarea) {
    NodoArbol* nodo = new NodoArbol("<tarea>");

    // tarea
    consume(TokenType::TAREA, "Se esperaba 'tarea'");
    nodo->hijos.push_back(new NodoArbol("tarea", true));

    // :
    consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'tarea'");
    nodo->hijos.push_back(new NodoArbol(":", true));

    // CADENA (nombre tarea)
    Token nombre = consume(TokenType::CADENA, "Se esperaba el nombre de la tarea");
    tarea.nombre = nombre.lexema;
    nodo->hijos.push_back(new NodoArbol("\"" + nombre.lexema + "\"", true));

    // [
    consume(TokenType::CORCHETE_ABRE, "Se esperaba '['");
    nodo->hijos.push_back(new NodoArbol("[", true));

    // <atributos>
    NodoArbol* atributos = parseAtributos(tarea.atributos);
    nodo->hijos.push_back(atributos);

    // ]
    consume(TokenType::CORCHETE_CIERRA, "Se esperaba ']'");
    nodo->hijos.push_back(new NodoArbol("]", true));

    return nodo;
}

// <atributos> ::= <atributo> "," <atributos> | <atributo>
NodoArbol* SyntaxAnalyzer::parseAtributos(Atributo& atributo) {
    NodoArbol* nodo = new NodoArbol("<atributos>");

    while (check(TokenType::PRIORIDAD) ||
           check(TokenType::RESPONSABLE) ||
           check(TokenType::FECHA_LIMITE)) {

        NodoArbol* a = parseAtributo(atributo);
        nodo->hijos.push_back(a);

        if (check(TokenType::COMA)) {
            nodo->hijos.push_back(new NodoArbol(",", true));
            advance();
        }
    }

    return nodo;
}

NodoArbol* SyntaxAnalyzer::parseAtributo(Atributo& atributo) {
    NodoArbol* nodo = new NodoArbol("<atributo>");

    if (check(TokenType::PRIORIDAD)) {
        advance();
        nodo->hijos.push_back(new NodoArbol("prioridad", true));
        consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'prioridad'");
        nodo->hijos.push_back(new NodoArbol(":", true));
        NodoArbol* prio = parsePrioridad(atributo.prioridad);
        nodo->hijos.push_back(prio);

    } else if (check(TokenType::RESPONSABLE)) {
        advance();
        nodo->hijos.push_back(new NodoArbol("responsable", true));
        consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'responsable'");
        nodo->hijos.push_back(new NodoArbol(":", true));
        Token resp = consume(TokenType::CADENA, "Se esperaba el nombre del responsable");
        atributo.responsable = resp.lexema;
        nodo->hijos.push_back(new NodoArbol("\"" + resp.lexema + "\"", true));

    } else if (check(TokenType::FECHA_LIMITE)) {
        advance();
        nodo->hijos.push_back(new NodoArbol("fecha_limite", true));
        consume(TokenType::DOS_PUNTOS, "Se esperaba ':' despues de 'fecha_limite'");
        nodo->hijos.push_back(new NodoArbol(":", true));
        Token fecha = consume(TokenType::FECHA, "Se esperaba una fecha en formato AAAA-MM-DD");
        atributo.fechaLimite = fecha.lexema;
        nodo->hijos.push_back(new NodoArbol(fecha.lexema, true));
    }

    return nodo;
}

// <prioridad> ::= ALTA | MEDIA | BAJA
NodoArbol* SyntaxAnalyzer::parsePrioridad(std::string& prioridad) {
    NodoArbol* nodo = new NodoArbol("<prioridad>");

    if (check(TokenType::ALTA)) {
        prioridad = "ALTA";
        nodo->hijos.push_back(new NodoArbol("ALTA", true));
        advance();
    } else if (check(TokenType::MEDIA)) {
        prioridad = "MEDIA";
        nodo->hijos.push_back(new NodoArbol("MEDIA", true));
        advance();
    } else if (check(TokenType::BAJA)) {
        prioridad = "BAJA";
        nodo->hijos.push_back(new NodoArbol("BAJA", true));
        advance();
    } else {
        Token t = currentToken();
        errorManager.agregarError(t.lexema, ErrorType::SINTACTICO,
                                  "Se esperaba ALTA, MEDIA o BAJA", t.linea, t.columna);
    }

    return nodo;
}

bool SyntaxAnalyzer::analizar() {
    raiz = parsePrograma();
    return !errorManager.hayErrores();
}

NodoArbol* SyntaxAnalyzer::getArbol() const { return raiz; }
Tablero SyntaxAnalyzer::getTablero() const { return tablero; }