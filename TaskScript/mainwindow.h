#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include "lexicalanalyzer.h"
#include "syntaxanalyzer.h"
#include "errormanager.h"
#include "reportgenerator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void cargarArchivo();
    void analizarArchivo();
    void abrirReporte1();
    void abrirReporte2();
    void abrirReporte3();
    void abrirGraphviz();

private:
    // Widgets principales
    QTextEdit*     editorCodigo;
    QTableWidget*  tablaTokens;
    QTableWidget*  tablaErrores;
    QTabWidget*    tabWidget;
    QLabel*        lblEstado;

    // Botones
    QPushButton* btnCargar;
    QPushButton* btnAnalizar;
    QPushButton* btnReporte1;
    QPushButton* btnReporte2;
    QPushButton* btnReporte3;
    QPushButton* btnGraphviz;

    // Datos
    QString archivoActual;
    QString outputDir;

    // Metodos privados
    void setupUI();
    void setupTablaTokens();
    void setupTablaErrores();
    void llenarTablaTokens(const std::vector<Token>& tokens);
    void llenarTablaErrores(const ErrorManager& em);
    void setEstado(const QString& msg, const QString& color);
};

#endif