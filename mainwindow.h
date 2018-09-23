#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "spinboxaction.h"
#include <QStandardItemModel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onDimsChanged(int dims);
private slots:
    void onDataChanged(QStandardItem* item);
private:
    Ui::MainWindow *ui;
    SpinBoxAction *_spinAction;
    QStandardItemModel *model;
    quint16 _dims;
    void setUpRotationWidget();
};

#endif // MAINWINDOW_H
