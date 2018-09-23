#ifndef NDIMSCENE_H
#define NDIMSCENE_H

#include <QWidget>
#include <QGraphicsView>
#include "ndimpoint.h"
#include <QTimer>
class ndimscene : public QGraphicsView
{
public:
    explicit ndimscene(QWidget* parent = nullptr, quint16 dims=3);
public slots:
    void startRotation();
    void stopRotation();
    void onSetDims(int dimensions);
private:
    quint32 _dims;
    QTimer* timer;
    QVector<NDimNode*> _points;
    QVector<Edge*> _edges;
    void clear();
    void setDims(quint16 dimensions);
private slots:
    void doStep();

};

#endif // NDIMSCENE_H
