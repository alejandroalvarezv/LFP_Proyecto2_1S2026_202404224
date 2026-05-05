#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include "token.h"
#include "errormanager.h"
#include <string>
#include <vector>

class LexicalAnalyzer {
public:
    LexicalAnalyzer(const std::string& input, ErrorManager& errorManager);

    std::vector<Token> analizar();

private:
    std::string source;
    int pos;
    int linea;
    int columna;
    ErrorManager& errorManager;

    char currentChar();
    char peek(int offset = 1);
    void advance();
    void skipWhitespace();
    void skipComment();

    Token readString();
    Token readNumberOrDate();
    Token readIdentifierOrKeyword();
    TokenType checkKeyword(const std::string& word);
    bool isDateFormat(const std::string& text);
};

#endif