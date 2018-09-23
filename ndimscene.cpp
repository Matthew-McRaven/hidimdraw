#include "ndimscene.h"
#include <cmath>
#include <qdebug.h>
#include <bitset>
#include <qvector.h>
static const int dist = 100;
ndimscene::ndimscene(QWidget* parent, quint16 dims): QGraphicsView(parent),
    timer(new QTimer(this)), _dims(dims), _points(), _rotMatrices(QVector<double>(21)),
    _idxToDims(QVector<std::tuple<quint16, quint16>>(22))
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-20, -20, 20, 20);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(2), qreal(2));
    setMinimumSize(300, 300);

    setDims(dims);
    connect(timer,&QTimer::timeout,this,&ndimscene::doStep);
    timer->setInterval(20);
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
        pt->project(tempInput,tempRet);
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
QVector<double> tempRet = QVector<double>(2), tempInput = QVector<double>(8);
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
        _points[it]->project(tempInput, tempRet);
    }
    this->setEnabled(true);
}
