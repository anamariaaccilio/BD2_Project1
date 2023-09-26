#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Parser.h"
#include <filesystem>

namespace fs = std::filesystem;

void highlightReservedWords(QTextEdit* textEdit) {
    QTextCharFormat keywordFormat;
    QColor color("#1584C8"); // Reemplaza "#RRGGBB" con el valor hexadecimal del color que desees
    keywordFormat.setForeground(color);
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList palabrasReservadas;
    palabrasReservadas << "SELECT" << "FROM" << "WHERE" << "USING" << "BETWEEN" << "INSERT" << "INTO" << "VALUES" << "CREATE" << "FILE" << "DELETE";

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

    QPixmap background("/Users/diegopachecoferrel/Documents/AED/BD2/capi.jpg"); // Reemplaza 'ruta_de_la_imagen.jpg' con la ruta de tu imagen
    ui->label->setPixmap(background);
    ui->label->setScaledContents(true);

    QTextCharFormat keywordFormat;
    QFont font1("arial"); // Reemplaza "NombreDeLaFuente" con el nombre de la fuente que desees
    font1.setBold(false); // Establece la negrita en falso para quitar la negrita
    keywordFormat.setFont(font1);

    QColor textColor("black"); // Establece el color del texto
    keywordFormat.setForeground(textColor);

    ui->tablas->setStyleSheet("background-color: white;"); // Elimina el cambio de color del texto en la hoja de estilo
    ui->tablas->setCurrentCharFormat(keywordFormat);


    ui->query->setStyleSheet("background-color: white; color: black;");


    fs::path carpeta = "/Users/diegopachecoferrel/Documents/AED/BD2/tablas/";

    for (const auto& archivo : fs::directory_iterator(carpeta)) {
        std::string nombreBase = archivo.path().stem().string();
        if (nombreBase.find("_aux") == std::string::npos && nombreBase != ".DS_Store" && nombreBase != "metadata" && nombreBase.find("index") == std::string::npos) {
            ui->tablas->appendPlainText(QString::fromStdString(nombreBase));
        }
    }

    // Cambiar el fondo de las celdas a verde y el texto a blanco
    ui->tabla->setStyleSheet("QTableWidget { background-color: white; color: black; }");

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
    ui->tabla->setColumnCount(7);
    ui->tabla->setHorizontalHeaderLabels({"id", "district", "year", "month", "day", "vol", "street"});

    vector<Record> records;

    Parser parser(query);
    parser.parse(records);

    int rowCount = ui->tabla->rowCount();

    QTextEdit* _query = ui->query;
    highlightReservedWords(_query);

    ui->tablas->clear();
    fs::path carpeta = "/Users/diegopachecoferrel/Documents/AED/BD2/tablas/";
    for (const auto& archivo : fs::directory_iterator(carpeta)) {
        std::string nombreBase = archivo.path().stem().string();
        if (nombreBase.find("_aux") == std::string::npos && nombreBase != ".DS_Store" && nombreBase != "metadata" && nombreBase.find("index") == std::string::npos) {
            ui->tablas->appendPlainText(QString::fromStdString(nombreBase));
        }
    }

    for (const auto &elem : records) {
        QString id = QString::number(elem.id);
        QString district = QString::fromStdString(elem.district);
        QString year = QString::number(elem.year);
        QString month = QString::number(elem.month);
        QString day = QString::number(elem.day);
        QString vol = QString::number(elem.vol);
        QString street = QString::fromStdString(elem.get_street());
        ui->tabla->insertRow(rowCount);
        ui->tabla->setItem(ui->tabla->rowCount()-1, 0, new QTableWidgetItem(id));
        ui->tabla->setItem(ui->tabla->rowCount()-1, 1, new QTableWidgetItem(district));
        ui->tabla->setItem(ui->tabla->rowCount()-1, 2, new QTableWidgetItem(year));
        ui->tabla->setItem(ui->tabla->rowCount()-1, 3, new QTableWidgetItem(month));
        ui->tabla->setItem(ui->tabla->rowCount()-1, 4, new QTableWidgetItem(day));
        ui->tabla->setItem(ui->tabla->rowCount()-1, 5, new QTableWidgetItem(vol));
        ui->tabla->setItem(ui->tabla->rowCount()-1, 6, new QTableWidgetItem(street));
        rowCount++;
    }

    ui->records->setText(QString::number(records.size()));

}


void MainWindow::on_query_textChanged()
{

}

