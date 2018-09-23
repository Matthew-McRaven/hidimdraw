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
    void setRotation(quint16 dim1, quint16 dim2, double theta);
public slots:
    void startRotation();
    void stopRotation();
    void onSetDims(int dimensions);
private:
    quint32 _dims, _rotEntries;
    QTimer* timer;
    QVector<NDimNode*> _points;
    QVector<Edge*> _edges;
    QVector<double> _rotMatrices;
    QVector<std::tuple<quint16, quint16>> _idxToDims;
    void clear();
    void setDims(quint16 dimensions);
private slots:
    void doStep();

};

#endif // NDIMSCENE_H
