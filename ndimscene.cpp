#include "ndimscene.h"
#include <cmath>
#include <qdebug.h>
#include <bitset>
static const int dist = 100;
ndimscene::ndimscene(quint16 dims): timer(new QTimer(this)), _dims(dims)
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
    setMinimumSize(20, 20);
    setWindowTitle(tr("Elastic Nodes"));
    QVector<QColor> colies;
    double nextHue = 0.2f;
    for(int it=0; it<dims; it++) {
        QColor nextColor = QColor(0,0,0);
        nextColor.toHsl();
        nextHue+=.6135;
        if (nextHue>1)nextHue-=1;
        nextColor.setHslF(nextHue,.5f,.4f);
        colies.append(nextColor);
    }
    for(int it = 0; it < 1<<dims; it++)
    {
        QVector<double> points;
        for(int codeC = 0; codeC < dims; codeC ++) {
            points << (bool(it&(1<<codeC))?1:-1) * dist;
        }
        NDimPoint* pt = new NDimPoint(it, nullptr, dims, points);
        _points.append(pt);
        _points[it]->rotate(2,1,.65);
        pt->project();
        scene->addItem(pt);
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
                edge->fillColor = colies[log2(it^other)];
                edge->setDotted(std::bitset<sizeof(int)>(it>>(int)log2(it^other)).count()%2);
                scene->addItem(edge);
            }
        }
        others.clear();
    }
    connect(timer,&QTimer::timeout,this,&ndimscene::doStep);
    timer->setInterval(20);
    timer->start();
}

void ndimscene::doStep()
{
    this->setEnabled(false);
    for(int it = 0; it < 1<< _dims; it++)
    {
        _points[it]->rotate(2,1,.005);
        _points[it]->rotate(1,3,.005);
        //_points[it]->rotate(4,2,.02);
        //_points[it]->rotate(2,1,.005);
        //_points[it]->rotate(4,5,.01);
        //_points[it]->rotate(3,5,.02);
        //_points[it]->rotate(4,3,.01);
        //_points[it]->rotate(1,0,.005);
        _points[it]->project();
    }
    this->setEnabled(true);
}
