#include "errormanager.h"

ErrorManager::ErrorManager() : contador(0) {}

void ErrorManager::agregarError(std::string lexema, ErrorType tipo,
                                std::string descripcion, int linea, int columna) {
    contador++;
    errores.emplace_back(contador, lexema, tipo, descripcion, linea, columna);
}

bool ErrorManager::hayErrores() const {
    return !errores.empty();
}

int ErrorManager::totalErrores() const {
    return errores.size();
}

const std::vector<Error>& ErrorManager::getErrores() const {
    return errores;
}

void ErrorManager::limpiar() {
    errores.clear();
    contador = 0;
}