#include "mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QDir>
#include <QHeaderView>
#include <fstream>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    outputDir = QDir::homePath() + "/TaskScript_Reportes/";
    QDir().mkpath(outputDir);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    setWindowTitle("TaskScript - Analizador Lexico y Sintactico");
    setMinimumSize(1100, 700);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // ── Barra de botones superior ──
    QHBoxLayout* btnLayout = new QHBoxLayout();

    btnCargar   = new QPushButton("📂 Cargar Archivo .task");
    btnAnalizar = new QPushButton("▶ Analizar");
    btnReporte1 = new QPushButton("📋 Tablero Kanban");
    btnReporte2 = new QPushButton("👤 Carga Responsable");
    btnReporte3 = new QPushButton("🔤 Tokens y Errores");
    btnGraphviz = new QPushButton("🌳 Arbol Graphviz");

    btnAnalizar->setEnabled(false);
    btnReporte1->setEnabled(false);
    btnReporte2->setEnabled(false);
    btnReporte3->setEnabled(false);
    btnGraphviz->setEnabled(false);

    QString btnStyle = "QPushButton { padding: 8px 14px; border-radius: 6px; "
                       "background: #2E75B6; color: white; font-weight: bold; }"
                       "QPushButton:hover { background: #1a5a9a; }"
                       "QPushButton:disabled { background: #95a5a6; }";

    btnCargar->setStyleSheet(btnStyle);
    btnAnalizar->setStyleSheet(btnStyle);
    btnReporte1->setStyleSheet(btnStyle);
    btnReporte2->setStyleSheet(btnStyle);
    btnReporte3->setStyleSheet(btnStyle);
    btnGraphviz->setStyleSheet(btnStyle);

    btnLayout->addWidget(btnCargar);
    btnLayout->addWidget(btnAnalizar);
    btnLayout->addStretch();
    btnLayout->addWidget(btnReporte1);
    btnLayout->addWidget(btnReporte2);
    btnLayout->addWidget(btnReporte3);
    btnLayout->addWidget(btnGraphviz);

    mainLayout->addLayout(btnLayout);

    // ── Label de estado ──
    lblEstado = new QLabel("Carga un archivo .task para comenzar.");
    lblEstado->setStyleSheet("padding: 6px; background: #ecf0f1; border-radius: 4px;");
    lblEstado->setMaximumHeight(35);

    // ── Splitter: editor | tabs ──
    QSplitter* splitter = new QSplitter(Qt::Horizontal);

    // Editor de codigo
    editorCodigo = new QTextEdit();
    editorCodigo->setReadOnly(true);
    editorCodigo->setPlaceholderText("El contenido del archivo .task aparecera aqui...");
    editorCodigo->setStyleSheet("font-family: Consolas, monospace; font-size: 13px;");
    editorCodigo->setMinimumWidth(350);

    // Tabs de resultados
    tabWidget = new QTabWidget();
    setupTablaTokens();
    setupTablaErrores();

    splitter->addWidget(editorCodigo);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    mainLayout->addWidget(splitter);

    // ── Conexiones ──
    connect(btnCargar,   &QPushButton::clicked, this, &MainWindow::cargarArchivo);
    connect(btnAnalizar, &QPushButton::clicked, this, &MainWindow::analizarArchivo);
    connect(btnReporte1, &QPushButton::clicked, this, &MainWindow::abrirReporte1);
    connect(btnReporte2, &QPushButton::clicked, this, &MainWindow::abrirReporte2);
    connect(btnReporte3, &QPushButton::clicked, this, &MainWindow::abrirReporte3);
    connect(btnGraphviz, &QPushButton::clicked, this, &MainWindow::abrirGraphviz);
}

void MainWindow::setupTablaTokens() {
    tablaTokens = new QTableWidget();
    tablaTokens->setColumnCount(5);
    tablaTokens->setHorizontalHeaderLabels({"#", "Lexema", "Tipo", "Linea", "Columna"});
    tablaTokens->horizontalHeader()->setStretchLastSection(true);
    tablaTokens->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaTokens->setAlternatingRowColors(true);
    tabWidget->addTab(tablaTokens, "🔤 Tokens");
}

void MainWindow::setupTablaErrores() {
    tablaErrores = new QTableWidget();
    tablaErrores->setColumnCount(6);
    tablaErrores->setHorizontalHeaderLabels({"#", "Lexema", "Tipo", "Descripcion", "Linea", "Columna"});
    tablaErrores->horizontalHeader()->setStretchLastSection(true);
    tablaErrores->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaErrores->setAlternatingRowColors(true);
    tabWidget->addTab(tablaErrores, "⚠ Errores");
}

void MainWindow::cargarArchivo() {
    QString path = QFileDialog::getOpenFileName(this, "Abrir archivo .task",
                                                QDir::homePath(), "TaskScript (*.task)");
    if (path.isEmpty()) return;

    archivoActual = path;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QTextStream in(&file);
    editorCodigo->setPlainText(in.readAll());
    file.close();

    btnAnalizar->setEnabled(true);
    setEstado("Archivo cargado: " + path, "#2980b9");
}

void MainWindow::analizarArchivo() {
    QString contenido = editorCodigo->toPlainText();
    if (contenido.isEmpty()) return;

    std::string source = contenido.toStdString();

    // Limpiar tablas
    tablaTokens->setRowCount(0);
    tablaErrores->setRowCount(0);

    // Analisis lexico
    ErrorManager errorManager;
    LexicalAnalyzer lexer(source, errorManager);
    std::vector<Token> tokens = lexer.analizar();

    // Analisis sintactico
    SyntaxAnalyzer parser(tokens, errorManager);
    bool exito = parser.analizar();

    // Llenar tablas
    llenarTablaTokens(tokens);
    llenarTablaErrores(errorManager);

    // Generar reportes
    ReportGenerator generator(parser.getTablero(), tokens, errorManager, parser.getArbol());
    generator.generarReporte1((outputDir + "reporte1_kanban.html").toStdString());
    generator.generarReporte2((outputDir + "reporte2_responsable.html").toStdString());
    generator.generarReporte3((outputDir + "reporte3_tokens.html").toStdString());
    generator.generarGraphviz((outputDir + "arbol.dot").toStdString());

    // Habilitar botones de reportes
    btnReporte1->setEnabled(true);
    btnReporte2->setEnabled(true);
    btnReporte3->setEnabled(true);
    btnGraphviz->setEnabled(true);

    if (exito) {
        setEstado("Analisis exitoso. Reportes generados en: " + outputDir, "#27ae60");
    } else {
        int errores = errorManager.totalErrores();
        setEstado("Analisis completado con " + QString::number(errores) + " error(es). Revisa la tabla de errores.", "#e74c3c");
    }

    tabWidget->setCurrentIndex(errorManager.hayErrores() ? 1 : 0);
}

void MainWindow::llenarTablaTokens(const std::vector<Token>& tokens) {
    int row = 0;
    for (const auto& tok : tokens) {
        if (tok.type == TokenType::FIN_ARCHIVO) continue;
        tablaTokens->insertRow(row);
        tablaTokens->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        tablaTokens->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(tok.lexema)));

        // Tipo como string
        QString tipo;
        switch(tok.type) {
        case TokenType::TABLERO:         tipo = "TABLERO"; break;
        case TokenType::COLUMNA:         tipo = "COLUMNA"; break;
        case TokenType::TAREA:           tipo = "TAREA"; break;
        case TokenType::PRIORIDAD:       tipo = "PRIORIDAD"; break;
        case TokenType::RESPONSABLE:     tipo = "RESPONSABLE"; break;
        case TokenType::FECHA_LIMITE:    tipo = "FECHA_LIMITE"; break;
        case TokenType::ALTA:            tipo = "ALTA"; break;
        case TokenType::MEDIA:           tipo = "MEDIA"; break;
        case TokenType::BAJA:            tipo = "BAJA"; break;
        case TokenType::CADENA:          tipo = "CADENA"; break;
        case TokenType::ENTERO:          tipo = "ENTERO"; break;
        case TokenType::FECHA:           tipo = "FECHA"; break;
        case TokenType::LLAVE_ABRE:      tipo = "LLAVE_ABRE"; break;
        case TokenType::LLAVE_CIERRA:    tipo = "LLAVE_CIERRA"; break;
        case TokenType::CORCHETE_ABRE:   tipo = "CORCHETE_ABRE"; break;
        case TokenType::CORCHETE_CIERRA: tipo = "CORCHETE_CIERRA"; break;
        case TokenType::DOS_PUNTOS:      tipo = "DOS_PUNTOS"; break;
        case TokenType::COMA:            tipo = "COMA"; break;
        case TokenType::PUNTO_COMA:      tipo = "PUNTO_COMA"; break;
        default:                         tipo = "ERROR"; break;
        }

        tablaTokens->setItem(row, 2, new QTableWidgetItem(tipo));
        tablaTokens->setItem(row, 3, new QTableWidgetItem(QString::number(tok.linea)));
        tablaTokens->setItem(row, 4, new QTableWidgetItem(QString::number(tok.columna)));
        row++;
    }
}

void MainWindow::llenarTablaErrores(const ErrorManager& em) {
    int row = 0;
    for (const auto& err : em.getErrores()) {
        tablaErrores->insertRow(row);
        tablaErrores->setItem(row, 0, new QTableWidgetItem(QString::number(err.numero)));
        tablaErrores->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(err.lexema)));
        tablaErrores->setItem(row, 2, new QTableWidgetItem(
                                          err.tipo == ErrorType::LEXICO ? "Lexico" : "Sintactico"));
        tablaErrores->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(err.descripcion)));
        tablaErrores->setItem(row, 4, new QTableWidgetItem(QString::number(err.linea)));
        tablaErrores->setItem(row, 5, new QTableWidgetItem(QString::number(err.columna)));

        // Color rojo para errores
        for (int col = 0; col < 6; col++)
            tablaErrores->item(row, col)->setForeground(QColor("#e74c3c"));
        row++;
    }
}

void MainWindow::setEstado(const QString& msg, const QString& color) {
    lblEstado->setText(msg);
    lblEstado->setStyleSheet("padding: 6px; background: " + color +
                             "; color: white; border-radius: 4px; font-weight: bold;");
}

void MainWindow::abrirReporte1() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(outputDir + "reporte1_kanban.html"));
}

void MainWindow::abrirReporte2() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(outputDir + "reporte2_responsable.html"));
}

void MainWindow::abrirReporte3() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(outputDir + "reporte3_tokens.html"));
}

void MainWindow::abrirGraphviz() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(outputDir + "arbol.dot"));
}