#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include "syntaxanalyzer.h"
#include "token.h"
#include "errormanager.h"
#include <string>
#include <vector>

class ReportGenerator {
public:
    ReportGenerator(const Tablero& tablero,
                    const std::vector<Token>& tokens,
                    const ErrorManager& errorManager,
                    NodoArbol* arbol);

    void generarReporte1(const std::string& outputPath); // Tablero Kanban
    void generarReporte2(const std::string& outputPath); // Carga por Responsable
    void generarReporte3(const std::string& outputPath); // Tabla de Tokens y Errores
    void generarGraphviz(const std::string& outputPath); // Arbol de derivacion

private:
    Tablero tablero;
    std::vector<Token> tokens;
    const ErrorManager& errorManager;
    NodoArbol* arbol;

    std::string colorPrioridad(const std::string& prioridad);
    std::string tokenTypeToString(TokenType type);
    void generarNodosGraphviz(NodoArbol* nodo, int& contador, std::string& resultado);
};

#endif