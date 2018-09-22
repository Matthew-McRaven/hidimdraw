#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton_Start, &QPushButton::clicked, ui->graphWidget, &ndimscene::startRotation);
    connect(ui->pushButton_Stop, &QPushButton::clicked, ui->graphWidget, &ndimscene::stopRotation);
}

MainWindow::~MainWindow()
{
    delete ui;
}

