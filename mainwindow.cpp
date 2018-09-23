#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "rottablevalidator.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    _spinAction= new SpinBoxAction("Dimensions", ui->menuBar);
    ui->menuShape->addAction(_spinAction);
    _spinAction->spinBox()->setValue(3);
    _spinAction->spinBox()->setMinimum(3);
    _spinAction->spinBox()->setMaximum(7);
    _dims = _spinAction->spinBox()->value();
    connect(ui->pushButton_Start, &QPushButton::clicked, ui->graphWidget, &ndimscene::startRotation);
    connect(ui->pushButton_Stop, &QPushButton::clicked, ui->graphWidget, &ndimscene::stopRotation);
    connect(_spinAction->spinBox(), static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::onDimsChanged);
    model->setColumnCount(2);
    ui->tableView_Rots->setModel(model);
    setUpRotationWidget();
    RotTableDelegator *del = new RotTableDelegator(ui->tableView_Rots);
    ui->tableView_Rots->setItemDelegate(del);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onDimsChanged(int dims)
{
    _dims = dims;
    ui->graphWidget->stopRotation();
    ui->graphWidget->onSetDims(dims);
    setUpRotationWidget();
}

void MainWindow::onDataChanged(QStandardItem *item)
{
    qDebug() << item->index();
    qDebug() << item->data(Qt::DisplayRole);
    QString name = model->data(model->index(item->row(),0)).toString();
    ui->graphWidget->setRotation(name[0].toLatin1()-'0',
            name[1].toLatin1()-'0',
            item->data(Qt::DisplayRole).toDouble()/100);
}

void MainWindow::setUpRotationWidget()
{
    disconnect(model, &QStandardItemModel::itemChanged, this, &MainWindow::onDataChanged);
    model->clear();
    int nFact=1, nmkFact;
    for(int it=2; it<=_dims; it++) {
        nFact*=it;
    }
    nmkFact = nFact/_dims;
    nmkFact /= (_dims-1);

    model->setRowCount(nFact/(2*nmkFact));
    //First Dim
    for(int fDim=0, row=0; fDim<_dims; fDim++) {
        //Second Dim
        for(int sDim=fDim+1; sDim<_dims; sDim++) {
            model->setItem(row, 0, new QStandardItem(QString("%0%1").arg(fDim).arg(sDim)));
            model->setItem(row, 1, new QStandardItem("0.0"));
            row++;
        }
    }
    ui->tableView_Rots->resizeColumnsToContents();
    ui->tableView_Rots->resizeRowsToContents();
    ui->tableView_Rots->horizontalHeader()->setStretchLastSection(true);
    connect(model, &QStandardItemModel::itemChanged, this, &MainWindow::onDataChanged);
}

