#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _spinAction= new SpinBoxAction("Dimensions", ui->menuBar);
    ui->menuShape->addAction(_spinAction);
    _spinAction->spinBox()->setValue(3);
    connect(ui->pushButton_Start, &QPushButton::clicked, ui->graphWidget, &ndimscene::startRotation);
    connect(ui->pushButton_Stop, &QPushButton::clicked, ui->graphWidget, &ndimscene::stopRotation);
    connect(_spinAction->spinBox(), static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->graphWidget, &ndimscene::onSetDims);
    _spinAction->spinBox()->setMinimum(3);
    _spinAction->spinBox()->setMaximum(7);
}

MainWindow::~MainWindow()
{
    delete ui;
}

