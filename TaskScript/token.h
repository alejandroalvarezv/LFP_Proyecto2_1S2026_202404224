#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    TABLERO, COLUMNA, TAREA, PRIORIDAD, RESPONSABLE, FECHA_LIMITE,
    ALTA, MEDIA, BAJA,
    CADENA, ENTERO, FECHA,
    LLAVE_ABRE,
    LLAVE_CIERRA,
    CORCHETE_ABRE,
    CORCHETE_CIERRA,
    DOS_PUNTOS,
    COMA,
    PUNTO_COMA,
    // Especiales
    FIN_ARCHIVO,
    ERROR_TOKEN
};

struct Token {
    TokenType type;
    std::string lexema;
    int linea;
    int columna;

    Token(TokenType t, std::string lex, int lin, int col)
        : type(t), lexema(lex), linea(lin), columna(col) {}
};

#endif