#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Parser.h"
#include <filesystem>

namespace fs = std::filesystem;

void highlightReservedWords(QTextEdit* textEdit) {
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::green); // Color para palabras reservadas (azul en este caso)
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList palabrasReservadas;
    palabrasReservadas << "SELECT" << "FROM" << "WHERE" << "USING" << "BETWEEN" << "INSERT" << "INTO" << "VALUES";

    foreach (const QString& palabraReservada, palabrasReservadas) {
        QRegularExpression exp("\\b" + palabraReservada + "\\b"); // Coincidir con palabras completas
        QTextCursor cursor(textEdit->document());

        while (!cursor.isNull()) {
            cursor = textEdit->document()->find(exp, cursor);
            if (!cursor.isNull()) {
                cursor.mergeCharFormat(keywordFormat);
            }
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fs::path carpeta = "/Users/diegopachecoferrel/Documents/AED/BD2/tablas/";

    for (const auto& archivo : fs::directory_iterator(carpeta)) {
        std::string nombreBase = archivo.path().stem().string();
        if (nombreBase.find("_aux") == std::string::npos && nombreBase != ".DS_Store") {
            ui->tablas->appendPlainText(QString::fromStdString(nombreBase));
        }
    }

    // Cambiar el fondo de las celdas a verde y el texto a blanco
    ui->tabla->setStyleSheet("QTableWidget { background-color: grey; color: black; }");

    QFont font("Arial", 15), font2("Arial", 15); // Cambia la fuente a Arial con tamaño 12
    ui->tabla->setFont(font);

    ui->query->setFont(font2);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_send_clicked()
{
    QString input = ui->query->toPlainText();
    string query = input.toStdString();

    ui->tabla->setRowCount(0);
    ui->tabla->setColumnCount(3);
    ui->tabla->setHorizontalHeaderLabels({"Nombre", "Carrera", "Ciclo"});

    vector<Record> records;

    Parser parser(query);
    parser.parse(records);

    int rowCount = ui->tabla->rowCount();

    QTextEdit* _query = ui->query;
    highlightReservedWords(_query);

    for (const auto &elem : records) {
        QString nombre = QString::fromStdString(elem.getKey());
        QString carrera = QString::fromStdString(elem.get_carrera());
        ui->tabla->insertRow(rowCount);
        ui->tabla->setItem(ui->tabla->rowCount()-1, 0, new QTableWidgetItem(nombre));
        ui->tabla->setItem(ui->tabla->rowCount()-1, 1, new QTableWidgetItem(carrera));
        ui->tabla->setItem(ui->tabla->rowCount()-1, 2, new QTableWidgetItem(QString::number(elem.get_ciclo())));
        rowCount++;
    }

    ui->records->setText(QString::number(records.size()));

}


void MainWindow::on_query_textChanged()
{

}

