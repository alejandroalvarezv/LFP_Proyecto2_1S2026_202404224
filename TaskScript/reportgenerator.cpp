#include "reportgenerator.h"
#include <fstream>
#include <sstream>
#include <map>

ReportGenerator::ReportGenerator(const Tablero& t, const std::vector<Token>& tk,
                                 const ErrorManager& em, NodoArbol* arbol)
    : tablero(t), tokens(tk), errorManager(em), arbol(arbol) {}

std::string ReportGenerator::colorPrioridad(const std::string& p) {
    if (p == "ALTA")  return "#e74c3c";
    if (p == "MEDIA") return "#f39c12";
    if (p == "BAJA")  return "#27ae60";
    return "#95a5a6";
}

std::string ReportGenerator::tokenTypeToString(TokenType type) {
    switch(type) {
    case TokenType::TABLERO:          return "TABLERO";
    case TokenType::COLUMNA:          return "COLUMNA";
    case TokenType::TAREA:            return "TAREA";
    case TokenType::PRIORIDAD:        return "PRIORIDAD";
    case TokenType::RESPONSABLE:      return "RESPONSABLE";
    case TokenType::FECHA_LIMITE:     return "FECHA_LIMITE";
    case TokenType::ALTA:             return "ALTA";
    case TokenType::MEDIA:            return "MEDIA";
    case TokenType::BAJA:             return "BAJA";
    case TokenType::CADENA:           return "CADENA";
    case TokenType::ENTERO:           return "ENTERO";
    case TokenType::FECHA:            return "FECHA";
    case TokenType::LLAVE_ABRE:       return "LLAVE_ABRE";
    case TokenType::LLAVE_CIERRA:     return "LLAVE_CIERRA";
    case TokenType::CORCHETE_ABRE:    return "CORCHETE_ABRE";
    case TokenType::CORCHETE_CIERRA:  return "CORCHETE_CIERRA";
    case TokenType::DOS_PUNTOS:       return "DOS_PUNTOS";
    case TokenType::COMA:             return "COMA";
    case TokenType::PUNTO_COMA:       return "PUNTO_COMA";
    case TokenType::FIN_ARCHIVO:      return "FIN_ARCHIVO";
    default:                          return "ERROR";
    }
}

std::string escaparLabel(const std::string& texto) {
    std::string resultado = "";
    for (char c : texto) {
        if (c == '"') resultado += "\\\"";
        else resultado += c;
    }
    return resultado;
}

// REPORTE 1 - Tablero Kanban Visual
void ReportGenerator::generarReporte1(const std::string& outputPath) {
    std::ostringstream html;
    html << R"(<!DOCTYPE html><html lang="es"><head><meta charset="UTF-8">
<title>Tablero Kanban</title><style>
body{font-family:Arial,sans-serif;background:#f0f2f5;margin:0;padding:20px;}
h1{color:#2c3e50;text-align:center;}
.board{display:flex;gap:20px;overflow-x:auto;padding:10px;}
.column{background:#e2e8f0;border-radius:10px;padding:15px;min-width:250px;flex:1;}
.column h2{font-size:16px;color:#2d3748;margin-bottom:10px;}
.card{background:white;border-radius:8px;padding:12px;margin-bottom:10px;box-shadow:0 2px 4px rgba(0,0,0,0.1);}
.card h3{margin:0 0 8px 0;font-size:14px;color:#2d3748;}
.badge{display:inline-block;padding:3px 10px;border-radius:12px;color:white;font-size:12px;font-weight:bold;margin-bottom:6px;}
.fecha{font-size:12px;color:#718096;margin:4px 0;}
.responsable{font-size:12px;background:#edf2f7;padding:2px 8px;border-radius:10px;display:inline-block;}
.col-count{font-size:12px;color:#718096;font-weight:normal;}
</style></head><body>)";

    html << "<h1>Tablero: " << tablero.nombre << "</h1><div class='board'>";

    for (const auto& col : tablero.columnas) {
        html << "<div class='column'><h2>" << col.nombre
             << " <span class='col-count'>(" << col.tareas.size() << ")</span></h2>";

        for (const auto& tarea : col.tareas) {
            std::string color = colorPrioridad(tarea.atributos.prioridad);
            html << "<div class='card'>"
                 << "<h3>" << tarea.nombre << "</h3>"
                 << "<span class='badge' style='background:" << color << "'>"
                 << tarea.atributos.prioridad << "</span><br>"
                 << "<span class='fecha'>Fecha limite: " << tarea.atributos.fechaLimite << "</span><br>"
                 << "<span class='responsable'>" << tarea.atributos.responsable << "</span>"
                 << "</div>";
        }
        html << "</div>";
    }

    html << "</div></body></html>";

    std::ofstream file(outputPath);
    file << html.str();
    file.close();
}

// REPORTE 2 - Carga por Responsable
void ReportGenerator::generarReporte2(const std::string& outputPath) {
    std::map<std::string, std::map<std::string,int>> carga;
    int totalTareas = 0;

    for (const auto& col : tablero.columnas) {
        for (const auto& tarea : col.tareas) {
            carga[tarea.atributos.responsable][tarea.atributos.prioridad]++;
            totalTareas++;
        }
    }

    std::ostringstream html;
    html << R"(<!DOCTYPE html><html lang="es"><head><meta charset="UTF-8">
<title>Carga por Responsable</title><style>
body{font-family:Arial,sans-serif;background:#f0f2f5;margin:0;padding:20px;}
h1{color:#2c3e50;text-align:center;}
.container{max-width:900px;margin:0 auto;background:white;border-radius:10px;padding:20px;box-shadow:0 2px 8px rgba(0,0,0,0.1);}
table{width:100%;border-collapse:collapse;}
th{background:#2E75B6;color:white;padding:12px;text-align:left;}
td{padding:10px;border-bottom:1px solid #e2e8f0;}
tr:hover{background:#f7fafc;}
.bar-container{background:#e2e8f0;border-radius:10px;height:20px;width:200px;}
.bar{background:#2E75B6;height:20px;border-radius:10px;display:flex;align-items:center;justify-content:center;color:white;font-size:11px;font-weight:bold;}
.alta{color:#e74c3c;font-weight:bold;}
.media{color:#f39c12;font-weight:bold;}
.baja{color:#27ae60;font-weight:bold;}
</style></head><body>)";

    html << "<div class='container'><h1>Carga por Responsable</h1><table>"
         << "<tr><th>Responsable</th><th>Tareas Asignadas</th><th class='alta'>Alta</th>"
         << "<th class='media'>Media</th><th class='baja'>Baja</th><th>Distribucion</th></tr>";

    for (const auto& entry : carga) {
        if (entry.first.empty()) continue; // ← evita fila vacia
        int alta  = entry.second.count("ALTA")  ? entry.second.at("ALTA")  : 0;
        int media = entry.second.count("MEDIA") ? entry.second.at("MEDIA") : 0;
        int baja  = entry.second.count("BAJA")  ? entry.second.at("BAJA")  : 0;
        int total = alta + media + baja;
        int pct   = totalTareas > 0 ? (total * 100 / totalTareas) : 0;

        html << "<tr><td>" << entry.first << "</td><td>" << total << "</td>"
             << "<td class='alta'>" << alta << "</td>"
             << "<td class='media'>" << media << "</td>"
             << "<td class='baja'>" << baja << "</td>"
             << "<td><div class='bar-container'><div class='bar' style='width:"
             << pct << "%'>" << pct << "%</div></div></td></tr>";
    }

    html << "</table></div></body></html>";

    std::ofstream file(outputPath);
    file << html.str();
    file.close();
}

// REPORTE 3 - Tabla de Tokens y Errores
void ReportGenerator::generarReporte3(const std::string& outputPath) {
    std::ostringstream html;
    html << R"(<!DOCTYPE html><html lang="es"><head><meta charset="UTF-8">
<title>Tokens y Errores</title><style>
body{font-family:Arial,sans-serif;background:#f0f2f5;margin:0;padding:20px;}
h2{color:#2c3e50;}
.container{max-width:1000px;margin:0 auto;}
table{width:100%;border-collapse:collapse;margin-bottom:30px;background:white;border-radius:10px;overflow:hidden;box-shadow:0 2px 8px rgba(0,0,0,0.1);}
th{background:#2E75B6;color:white;padding:10px;}
td{padding:8px 10px;border-bottom:1px solid #e2e8f0;font-size:13px;}
tr:hover{background:#f7fafc;}
.error{color:#e74c3c;font-weight:bold;}
</style></head><body><div class='container'>)";

    html << "<h2>Tabla de Tokens</h2><table>"
         << "<tr><th>#</th><th>Lexema</th><th>Tipo</th><th>Linea</th><th>Columna</th></tr>";

    int i = 1;
    for (const auto& tok : tokens) {
        if (tok.type == TokenType::FIN_ARCHIVO) continue;
        html << "<tr><td>" << i++ << "</td><td>" << tok.lexema
             << "</td><td>" << tokenTypeToString(tok.type)
             << "</td><td>" << tok.linea
             << "</td><td>" << tok.columna << "</td></tr>";
    }
    html << "</table>";

    html << "<h2>Tabla de Errores</h2><table>"
         << "<tr><th>#</th><th>Lexema</th><th>Tipo</th><th>Descripcion</th><th>Linea</th><th>Columna</th></tr>";

    if (!errorManager.hayErrores()) {
        html << "<tr><td colspan='6' style='text-align:center;color:#27ae60;'>No se encontraron errores</td></tr>";
    } else {
        for (const auto& err : errorManager.getErrores()) {
            std::string tipo = (err.tipo == ErrorType::LEXICO) ? "Lexico" : "Sintactico";
            html << "<tr><td>" << err.numero << "</td><td>" << err.lexema
                 << "</td><td class='error'>" << tipo
                 << "</td><td>" << err.descripcion
                 << "</td><td>" << err.linea
                 << "</td><td>" << err.columna << "</td></tr>";
        }
    }
    html << "</table></div></body></html>";

    std::ofstream file(outputPath);
    file << html.str();
    file.close();
}

// GRAPHVIZ - Arbol de derivacion
void ReportGenerator::generarNodosGraphviz(NodoArbol* nodo, int& contador, std::string& resultado) {
    if (!nodo) return;
    int id = contador++;

    if (nodo->esTerminal) {
        resultado += "n" + std::to_string(id) + " [label=\"" + escaparLabel(nodo->etiqueta)
        + "\", fillcolor=\"#D6EAF8\", style=filled];\n";
    } else {
        resultado += "n" + std::to_string(id) + " [label=\"" + escaparLabel(nodo->etiqueta)
        + "\", fillcolor=\"#2E75B6\", fontcolor=white, style=filled];\n";
    }

    for (auto hijo : nodo->hijos) {
        int hijoId = contador;
        generarNodosGraphviz(hijo, contador, resultado);
        resultado += "n" + std::to_string(id) + " -> n" + std::to_string(hijoId) + ";\n";
    }
}

void ReportGenerator::generarGraphviz(const std::string& outputPath) {
    std::string contenido = "digraph ArbolDerivacion {\n";
    contenido += "  rankdir=TB;\n";
    contenido += "  node [shape=box, fontname=\"Arial\"];\n";

    int contador = 0;
    std::string nodos = "";
    generarNodosGraphviz(arbol, contador, nodos);
    contenido += nodos;
    contenido += "}\n";

    std::ofstream file(outputPath);
    file << contenido;
    file.close();
}