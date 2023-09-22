#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidgetItem>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    connect(ui->insertButton, SIGNAL(clicked()), this, SLOT(insertRecord()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteRecord()));
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(searchRecord()));


    ui->tableWidget->setColumnCount(2);
    QStringList headers;
    headers << "Campo1" << "Campo2";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::insertRecord()
{
    QString query = ui->queryInput->text();
}

void MainWindow::deleteRecord()
{
    QString query = ui->queryInput->text();

}

void MainWindow::searchRecord()
{

    QString query = ui->queryInput->text();

}
