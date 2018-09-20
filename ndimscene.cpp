#include "ndimscene.h"
#include <cmath>
#include <qdebug.h>
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

    for(int it = 0; it < 1<<dims; it++)
    {
        QVector<double> points;
        for(int codeC = 0; codeC < dims; codeC ++) {
            points << (bool(it&(1<<codeC))?1:-1) * dist;
        }
        NDimPoint* pt = new NDimPoint(it, nullptr, dims, points);
        _points.append(pt);
        pt->project();
        scene->addItem(pt);
        points.clear();
    }
    for(int it = 0; it < 1<<dims; it++)
    {
        int other;
        for(int codeC = 0; codeC < dims; codeC ++) {
            other = it^(1<<codeC);
            if(other < it) continue;
            else scene->addItem(new Edge(_points[it],_points[other]));
        }
    }
    connect(timer,&QTimer::timeout,this,&ndimscene::doStep);
    timer->setInterval(20);
    timer->start();
}

void ndimscene::doStep()
{
    for(int it = 0; it < 1<< _dims; it++)
    {
        _points[it]->rotate(1,2,.01);
        _points[it]->rotate(0,2,.02);
        _points[it]->project();
    }
}
