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
    void on_pushButton_Start_clicked();
    void on_pushButton_Stop_clicked();
    void on_actionReset_Rotation_triggered();
    void on_actionReset_Camera_triggered();
private slots:
    void onDataChanged(QStandardItem* item);
private:
    Ui::MainWindow *ui;
    SpinBoxAction *_spinAction;
    QStandardItemModel *_model;
    quint16 _dims;
    bool _isRunning = false;
    void setUpRotationWidget();
    void updateRunButtons();
};

#endif // MAINWINDOW_H
