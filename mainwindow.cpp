#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "rottablevalidator.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), _model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    _spinAction= new SpinBoxAction("Dimensions", ui->menuBar);
    ui->menuShape->addAction(_spinAction);
    _spinAction->spinBox()->setValue(3);
    _spinAction->spinBox()->setMinimum(3);
    _spinAction->spinBox()->setMaximum(7);
    _dims = _spinAction->spinBox()->value();
    connect(_spinAction->spinBox(), static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::onDimsChanged);
    _model->setColumnCount(2);
    ui->tableView_Rots->setModel(_model);
    setUpRotationWidget();
    RotTableDelegator *del = new RotTableDelegator(ui->tableView_Rots);
    ui->tableView_Rots->setItemDelegate(del);
    updateRunButtons();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onDimsChanged(int dims)
{
    _dims = dims;
    _isRunning = false;
    updateRunButtons();
    ui->graphWidget->stopRotation();
    ui->graphWidget->onSetDims(dims);
    setUpRotationWidget();
}

void MainWindow::on_pushButton_Start_clicked()
{
    _isRunning = true;
    ui->graphWidget->startRotation();
    updateRunButtons();
}

void MainWindow::on_pushButton_Stop_clicked()
{
    _isRunning = false;
    ui->graphWidget->stopRotation();
    updateRunButtons();
}

void MainWindow::on_actionReset_Rotation_triggered()
{
    ui->graphWidget->resetPosition();
    ui->graphWidget->repaint();
}

void MainWindow::on_actionReset_Camera_triggered()
{
    ui->graphWidget->resetCamera();
    ui->graphWidget->repaint();
}

void MainWindow::onDataChanged(QStandardItem *item)
{
    qDebug() << item->index();
    qDebug() << item->data(Qt::DisplayRole);
    QString name = _model->data(_model->index(item->row(),0)).toString();
    ui->graphWidget->setRotation(name[0].toLatin1()-'0',
            name[1].toLatin1()-'0',
            item->data(Qt::DisplayRole).toDouble()/100);
}

void MainWindow::setUpRotationWidget()
{
    disconnect(_model, &QStandardItemModel::itemChanged, this, &MainWindow::onDataChanged);
    _model->clear();
    int nFact=1, nmkFact;
    for(int it=2; it<=_dims; it++) {
        nFact*=it;
    }
    nmkFact = nFact/_dims;
    nmkFact /= (_dims-1);

    _model->setRowCount(nFact / (2 * nmkFact));
    //First Dim
    for(int fDim = 0, row=0; fDim < _dims; fDim++) {
        //Second Dim
        for(int sDim = fDim+1; sDim < _dims; sDim++) {
            _model->setItem(row, 0, new QStandardItem(QString("%0%1").arg(fDim).arg(sDim)));
            _model->setItem(row, 1, new QStandardItem("0.0"));
            row++;
        }
    }
    connect(_model, &QStandardItemModel::itemChanged, this, &MainWindow::onDataChanged);
    ui->tableView_Rots->resizeRowsToContents();
    ui->tableView_Rots->resizeColumnsToContents();
    //Seems redundant, but horizontal header will not stretch last column correctly one resize w/o disable+re-enable
    ui->tableView_Rots->horizontalHeader()->setStretchLastSection(false);
    ui->tableView_Rots->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::updateRunButtons()
{
    if(_isRunning) {
        ui->pushButton_Start->setEnabled(false);
        ui->pushButton_Stop->setEnabled(true);
    }
    else {
        ui->pushButton_Start->setEnabled(true);
        ui->pushButton_Stop->setEnabled(false);
    }
    ui->pushButton_Start->repaint();
    ui->pushButton_Stop->repaint();
}

