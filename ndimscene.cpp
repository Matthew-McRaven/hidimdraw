#include "ndimscene.h"
#include <cmath>
#include <qdebug.h>
#include <bitset>
#include <qvector.h>
#include <QGestureEvent>
#include <QScrollBar>
static const int dist = 100;
QVector<double> tempRet = QVector<double>(2), tempInput = QVector<double>(8);
ndimscene::ndimscene(QWidget* parent, quint16 dims): QGraphicsView(parent), _scaleFactor(2),
    timer(new QTimer(this)), _dims(dims), _points(), _rotMatrices(QVector<double>(21)), _xyOffsets(QVector<double>(2)),
    _idxToDims(QVector<std::tuple<quint16, quint16>>(22))
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200,-200,400,400);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(_scaleFactor), qreal(_scaleFactor));
    setMinimumSize(300, 300);
    setFocusPolicy(Qt::StrongFocus);

    setDims(dims);
    connect(timer,&QTimer::timeout,this,&ndimscene::doStep);
    timer->setInterval(20);
    this->grabGesture(Qt::PinchGesture); this->grabGesture(Qt::PanGesture);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setDragMode(DragMode::ScrollHandDrag);
}

void ndimscene::setRotation(quint16 dim1, quint16 dim2, double theta)
{
    if(dim1 == dim2) return;
    if(dim1 > dim2) {
        theta *= -1;
        quint16 temp = dim1;
        dim1 = dim2;
        dim2 = temp;
    }
    auto temp = std::tuple<quint16, quint16>(dim1, dim2);
    for(int it=0; it<_rotEntries; it++) {
        if (_idxToDims[it] == temp) {
            qDebug().noquote() << dim1 <<" wrt " << dim2 << " set to "<< theta;
            _rotMatrices[it] = theta;
            break;
        }
    }
}

void ndimscene::resetPosition()
{
    for(int it=0; it<_points.size(); it++){
        _points[it]->resetPosition();
        _points[it]->project(tempInput, tempRet, _xyOffsets);
    }
}

void ndimscene::resetCamera()
{
    _xyOffsets[0] = 0;
    _xyOffsets[1] = 0;
    for(int it = 0; it < 1<< _dims; it++)
        _points[it]->project(tempInput, tempRet, _xyOffsets);

}

void ndimscene::resetRotation()
{
    for(int it=0; it<_rotEntries; it++) _rotMatrices[it]=0;
}

void ndimscene::startRotation()
{
    timer->start();
}

void ndimscene::stopRotation()
{
    timer->stop();
}

void ndimscene::onSetDims(int dimensions)
{
    clear();
    setDims(dimensions);
}

void ndimscene::zoomIn()
{
    scaleView(qreal(1.2));
}

void ndimscene::zoomOut()
{
    scaleView(1/qreal(1.2));
}

bool ndimscene::event(QEvent *event)
{
    switch(event->type()){
    case QEvent::Gesture:
        gestureEvent(static_cast<QGestureEvent*>(event));
        return true;
        break;
    default:
        return QGraphicsView::event(event);
    }
}

void ndimscene::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        scaleView(static_cast<QPinchGesture *>(pinch)->scaleFactor());
    }
}

void ndimscene::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        //Move view up
        _xyOffsets[1]+=10;
        doStep();
        setFocus();
        break;
    case Qt::Key_Down:
        _xyOffsets[1]-=10;
        doStep();
        setFocus();
        //Move view down
        break;
    case Qt::Key_Left:
        //Move view left
        _xyOffsets[0]+=10;
        doStep();
        setFocus();
        break;
    case Qt::Key_Right:
        //Move view right
        _xyOffsets[0]-=10;
        doStep();
        setFocus();
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void ndimscene::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    QGraphicsView::mousePressEvent(event);
}
#if QT_CONFIG(wheelevent)
void ndimscene::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
#endif

void ndimscene::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;
    scale(scaleFactor, scaleFactor);
}

void ndimscene::clear()
{
    _points.clear();
    _edges.clear();
    for(int it=0; it<_points.size(); it++) {
        delete _points[it];
    }
    for(int it=0; it<_edges.size(); it++) {
        delete _edges[it];
    }
}

void ndimscene::setDims(quint16 dims)
{
    _dims = dims;
    scene()->clear();
    QVector<QColor> colors;
    double nextHue = 0.2f;
    for(int it=0; it<dims; it++) {
        QColor nextColor;
        nextHue+=.6135;
        if (nextHue>1) nextHue-=1;
        nextColor.setHslF(nextHue,.5f,.4f);
        colors.append(nextColor);
    }
    QVector<double> tempRet = QVector<double>(2), tempInput = QVector<double>(dims);
    for(int it = 0; it < 1<<dims; it++)
    {
        QVector<double> points;
        for(int codeC = 0; codeC < dims; codeC ++) {
            points << (bool(it & (1 << codeC))? 1 : -1) * dist;
        }
        NDimNode* pt = new NDimNode(it, nullptr, dims, points);
        _points.append(pt);
        _points[it]->rotate(2,1,.65);
        pt->project(tempInput,tempRet, _xyOffsets);
        scene()->addItem(pt);
        points.clear();
    }
    QList<int> others;
    for(int it = 0; it < 1<<dims; it++)
    {
        int other;
        for(int codeC = 0; codeC < dims; codeC ++) {
            others.append(it^(1<<codeC));
        }
        qSort(others);
        for(int inner = 0; inner< others.length(); inner++)
        {
            other = others[inner];
            if(other < it) continue;
            else {
                Edge* edge = new Edge(_points[it],_points[other]);
                edge->cidx = (inner+std::bitset<sizeof(int)>(it).count())%dims;
                edge->fillColor = colors[log2(it^other)];
                edge->setDotted(std::bitset<sizeof(int)>(it>>(int)log2(it^other)).count()%2);
                scene()->addItem(edge);
            }
        }
        others.clear();
    }
    _rotEntries = 0;
    //First Dim
    for(int fDim=0; fDim<_dims; fDim++) {
        //Second Dim
        for(int sDim=fDim+1; sDim<_dims; sDim++) {
            if(_rotEntries> _idxToDims.size()) _idxToDims.append(std::tuple<quint16, quint16>(fDim, sDim));
            else _idxToDims[_rotEntries]=std::tuple<quint16, quint16>(fDim, sDim);
            _rotEntries++;
        }
    }
    for(int it=0; it<_rotEntries; it++) _rotMatrices[it] = 0;

}

void ndimscene::doStep()
{
    this->setEnabled(false);
    for(int it = 0; it < 1<< _dims; it++)
    {
        for(int rotDim=0; rotDim<_rotEntries; rotDim++) {
            if(_rotMatrices[rotDim]==0.0f) continue;
            else _points[it]->rotate(std::get<0>(_idxToDims[rotDim]),
                                     std::get<1>(_idxToDims[rotDim]),
                                     _rotMatrices[rotDim]);
        }
        _points[it]->project(tempInput, tempRet, _xyOffsets);
    }
    this->setEnabled(true);
}
