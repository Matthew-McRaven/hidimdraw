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
    colies.append(QColor(255,127,167));
    for(int it=1; it<dims; it++) {
        QColor nextColor = QColor(0,0,0);
        nextColor.toHsl();
        qreal temp = colies[it-1].hslHueF()+1.f/dims;
        if (temp>1)temp-=1;
        qDebug() << temp;
        nextColor.setHslF(temp,colies[it-1].saturationF(),colies[it-1].lightnessF());
        qDebug() << nextColor;
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
                qDebug()<<it<<other;
                edge->cidx = (inner+std::bitset<sizeof(int)>(it).count())%dims;
                edge->fillColor = colies[log2(it^other)];
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
        //_points[it]->rotate(4,0,.01);
        _points[it]->rotate(1,3,.005);
        //_points[it]->rotate(4,2,.02);
        _points[it]->rotate(2,1,.005);
        //_points[it]->rotate(4,5,.01);
        //_points[it]->rotate(3,5,.02);
        //_points[it]->rotate(4,3,.01);
        _points[it]->rotate(1,0,.005);
        _points[it]->project();
    }
    this->setEnabled(true);
}
