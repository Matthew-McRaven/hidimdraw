#ifndef NDIMSCENE_H
#define NDIMSCENE_H

#include <QWidget>
#include <QGraphicsView>
#include "ndimpoint.h"
#include <QTimer>
#include <QWheelEvent>
#include <QGestureEvent>
class ndimscene : public QGraphicsView
{
public:
    explicit ndimscene(QWidget* parent = nullptr, quint16 dims=3);
    void setRotation(quint16 dim1, quint16 dim2, double theta);
    void resetPosition();
    void resetCamera();
    void resetRotation();
public slots:
    void startRotation();
    void stopRotation();
    void onSetDims(int dimensions);
    void zoomIn();
    void zoomOut();
    //Override to support gestures
    bool event(QEvent *event) override;
    void gestureEvent(QGestureEvent* event);
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
protected slots:
    void scaleView(qreal scaleFactor);
private:
    quint32 _dims, _rotEntries;
    double _scaleFactor;
    QTimer* timer;
    QVector<NDimNode*> _points;
    QVector<Edge*> _edges;
    QVector<double> _rotMatrices, _xyOffsets;
    QVector<std::tuple<quint16, quint16>> _idxToDims;
    void clear();
    void setDims(quint16 dimensions);
private slots:
    void doStep();

};

#endif // NDIMSCENE_H
