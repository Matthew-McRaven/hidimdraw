#ifndef NDIMSCENE_H
#define NDIMSCENE_H

#include <QWidget>
#include <QGraphicsView>
#include "ndimpoint.h"
#include <QTimer>
class ndimscene : public QGraphicsView
{
public:
    explicit ndimscene(quint16 dims);
private:
    quint32 _dims;
    QTimer* timer;
    QVector<NDimPoint*> _points;
private slots:
    void doStep();
};

#endif // NDIMSCENE_H
