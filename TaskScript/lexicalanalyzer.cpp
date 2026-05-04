#include "lexicalanalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(const std::string& input, ErrorManager& em)
    : source(input), pos(0), linea(1), columna(1), errorManager(em) {}

char LexicalAnalyzer::currentChar() {
    if (pos < (int)source.size()) return source[pos];
    return '\0';
}

char LexicalAnalyzer::peek(int offset) {
    int p = pos + offset;
    if (p < (int)source.size()) return source[p];
    return '\0';
}

void LexicalAnalyzer::advance() {
    if (pos < (int)source.size()) {
        if (source[pos] == '\n') { linea++; columna = 1; }
        else columna++;
        pos++;
    }
}

void LexicalAnalyzer::skipWhitespace() {
    while (pos < (int)source.size() && isspace(currentChar()))
        advance();
}

TokenType LexicalAnalyzer::checkKeyword(const std::string& word) {
    if (word == "TABLERO")     return TokenType::TABLERO;
    if (word == "COLUMNA")     return TokenType::COLUMNA;
    if (word == "tarea")       return TokenType::TAREA;
    if (word == "prioridad")   return TokenType::PRIORIDAD;
    if (word == "responsable") return TokenType::RESPONSABLE;
    if (word == "fecha_limite")return TokenType::FECHA_LIMITE;
    if (word == "ALTA")        return TokenType::ALTA;
    if (word == "MEDIA")       return TokenType::MEDIA;
    if (word == "BAJA")        return TokenType::BAJA;
    return TokenType::ERROR_TOKEN;
}

bool LexicalAnalyzer::isDateFormat(const std::string& text) {
    if (text.size() != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (text[i] != '-') return false;
        } else {
            if (!isdigit(text[i])) return false;
        }
    }
     int mes = std::stoi(text.substr(5, 2));
    int dia = std::stoi(text.substr(8, 2));
    if (mes < 1 || mes > 12) return false;
    if (dia < 1 || dia > 31) return false;
    return true;
}

Token LexicalAnalyzer::readString() {
    int startLine = linea, startCol = columna;
    std::string result = "";
    advance(); // saltar la comilla inicial "

    while (pos < (int)source.size() && currentChar() != '"') {
        if (currentChar() == '\n') {
            errorManager.agregarError(result, ErrorType::LEXICO,
                                      "Cadena no cerrada antes del fin de linea", startLine, startCol);
            return Token(TokenType::ERROR_TOKEN, result, startLine, startCol);
        }
        result += currentChar();
        advance();
    }

    if (currentChar() == '"') {
        advance();
        return Token(TokenType::CADENA, result, startLine, startCol);
    }

    errorManager.agregarError(result, ErrorType::LEXICO,
                              "Cadena no cerrada al final del archivo", startLine, startCol);
    return Token(TokenType::ERROR_TOKEN, result, startLine, startCol);
}

Token LexicalAnalyzer::readNumberOrDate() {
    int startLine = linea, startCol = columna;
    std::string result = "";

    while (pos < (int)source.size() && (isdigit(currentChar()) || currentChar() == '-')) {
        if (currentChar() == '-' && result.size() != 4 && result.size() != 7)
            break;
        result += currentChar();
        advance();
    }

    // Tiene estructura de fecha AAAA-MM-DD
    if (result.size() == 10 && result[4] == '-' && result[7] == '-') {
        if (isDateFormat(result)) {
            return Token(TokenType::FECHA, result, startLine, startCol);
        } else {
            errorManager.agregarError(result, ErrorType::LEXICO,
                                      "Fecha con valores invalidos (mes 01-12, dia 01-31): " + result,
                                      startLine, startCol);
            return Token(TokenType::ERROR_TOKEN, result, startLine, startCol);
        }
    }

    // Es un numero entero
    std::string numStr = "";
    for (char c : result) if (isdigit(c)) numStr += c;
    return Token(TokenType::ENTERO, numStr, startLine, startCol);
}

Token LexicalAnalyzer::readIdentifierOrKeyword() {
    int startLine = linea, startCol = columna;
    std::string result = "";

    while (pos < (int)source.size() &&
           (isalnum(currentChar()) || currentChar() == '_')) {
        result += currentChar();
        advance();
    }

    TokenType type = checkKeyword(result);
    if (type == TokenType::ERROR_TOKEN) {
        errorManager.agregarError(result, ErrorType::LEXICO,
                                  "Identificador no reconocido: " + result, startLine, startCol);
    }
    return Token(type, result, startLine, startCol);
}

std::vector<Token> LexicalAnalyzer::analizar() {
    std::vector<Token> tokens;

    while (pos < (int)source.size()) {
        skipWhitespace();
        if (pos >= (int)source.size()) break;

        int startLine = linea, startCol = columna;
        char c = currentChar();

        if (c == '"') {
            tokens.push_back(readString());
        } else if (isdigit(c)) {
            tokens.push_back(readNumberOrDate());
        } else if (isalpha(c) || c == '_') {
            tokens.push_back(readIdentifierOrKeyword());
        } else {
            switch (c) {
            case '{': tokens.push_back(Token(TokenType::LLAVE_ABRE,    "{", startLine, startCol)); advance(); break;
            case '}': tokens.push_back(Token(TokenType::LLAVE_CIERRA,  "}", startLine, startCol)); advance(); break;
            case '[': tokens.push_back(Token(TokenType::CORCHETE_ABRE, "[", startLine, startCol)); advance(); break;
            case ']': tokens.push_back(Token(TokenType::CORCHETE_CIERRA,"]",startLine, startCol)); advance(); break;
            case ':': tokens.push_back(Token(TokenType::DOS_PUNTOS,    ":", startLine, startCol)); advance(); break;
            case ',': tokens.push_back(Token(TokenType::COMA,          ",", startLine, startCol)); advance(); break;
            case ';': tokens.push_back(Token(TokenType::PUNTO_COMA,    ";", startLine, startCol)); advance(); break;
            default:
                errorManager.agregarError(std::string(1,c), ErrorType::LEXICO,
                                          std::string("Caracter no reconocido: '") + c + "'", startLine, startCol);
                advance();
                break;
            }
        }
    }

    tokens.push_back(Token(TokenType::FIN_ARCHIVO, "EOF", linea, columna));
    return tokens;
}