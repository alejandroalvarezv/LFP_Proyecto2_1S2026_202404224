#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <string>
#include <vector>

enum class ErrorType {
    LEXICO,
    SINTACTICO
};

struct Error {
    int numero;
    std::string lexema;
    ErrorType tipo;
    std::string descripcion;
    int linea;
    int columna;

    Error(int num, std::string lex, ErrorType t, std::string desc, int lin, int col)
        : numero(num), lexema(lex), tipo(t), descripcion(desc), linea(lin), columna(col) {}
};

class ErrorManager {
public:
    ErrorManager();
    void agregarError(std::string lexema, ErrorType tipo, std::string descripcion, int linea, int columna);
    bool hayErrores() const;
    int totalErrores() const;
    const std::vector<Error>& getErrores() const;
    void limpiar();

private:
    std::vector<Error> errores;
    int contador;
};

#endif