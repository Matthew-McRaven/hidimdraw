#include "ndimpoint.h"
#include <numeric>
#include <limits>
#include <QPainter>
#include <qmath.h>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <bitset>
NDimPoint::NDimPoint(quint32 pointNum, QGraphicsItem *parent, quint16 dims, QVector<double> point):
    QGraphicsItem(parent), _pointNum(pointNum), _dims(dims), _points(point), _actualX(std::numeric_limits<float>::quiet_NaN())
{
}
const int cDist = 500;
QPainterPath NDimPoint::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void NDimPoint::rotate(quint16 d1, quint16 d2, double theta)
{
    if(theta == 0 || _dims == 2) return;
    else {
        //Remove cache (X, Y) values
        _actualX = std::numeric_limits<float>::quiet_NaN();
        //Now perform rotation
        double a = _points[d1], b = _points[d2];
        double nT = theta;
        _points[d1] = a*cos(nT) - b * sin(nT);
        _points[d2] = a*sin(nT) + b * cos(nT);
    }
    for(int it=0; it <_edges.length(); it++) _edges[it]->adjust();
}

void NDimPoint::setPoints(QVector<double> &&points)
{
    _actualX = std::numeric_limits<float>::quiet_NaN();
    if(points.size() != _dims) throw -1;
    _points = std::move(points);
}

void NDimPoint::addEdge(Edge * edge)
{
    _edges << edge;
    edge->adjust();
}

QRectF NDimPoint::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

void NDimPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);
    QColor color;
    if(_pointNum == 0) color = QColor("red");
    else if(_pointNum == (1<<_dims)-1) color = QColor("green");
    else if(std::bitset<sizeof(quint32)>(_pointNum).count()%2==1)color=QColor("blue");
    else color = QColor("orange");
    painter->setBrush(color);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
}

void NDimPoint::project()
{
    if(_dims != 2) {
        QVector<double> ret = QVector<double>(2);
        project(_dims-1, _points, ret);
        //qDebug() << ret;
        _actualX = ret[0];
        _actualY = ret[1];
        //Handle the actual projection
        this->setPos(QPoint((int)_actualX, (int)_actualY));
        for(int it=0; it<_edges.size();it++) _edges[it]->adjust();
    } else if(_dims == 2) {
        _actualX = _points[0];
        _actualY = _points[1];
        this->setPos(QPoint((int)_actualX, (int)_actualY));
        for(int it=0; it<_edges.size();it++) _edges[it]->adjust();
    }
    else return;
}

void NDimPoint::project(quint16 dim, QVector<double> inputs, QVector<double> &retVal) const
{
    double dx, dy, dz, dw;
    if(dim == 2) {
        double v;
        for(int it=0; it < dim; it++)
        {
            v = sqrt(inputs[it]*inputs[it]+(cDist+inputs[dim])*(cDist+inputs[dim]));
            retVal[it] = inputs[it]*cDist/(cDist+inputs[dim]);
        }
         qDebug() <<retVal;
    }
    else {
        double v;
        for(int it=0; it < dim; it++)
        {
            v = sqrt(inputs[it]*inputs[it]+(cDist+inputs[dim])*(cDist+inputs[dim]));
            inputs[it] = inputs[it]*cDist/(cDist+inputs[dim]);
        }
        project(dim-1, inputs, retVal);
    }
}

Edge::Edge(NDimPoint *sourceNode, NDimPoint *destNode)
    : arrowSize(10)
{
    setAcceptedMouseButtons(0);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
    adjust();
}

NDimPoint *Edge::sourceNode() const
{
    return source;
}

NDimPoint *Edge::destNode() const
{
    return dest;
}

void Edge::adjust()
{
    if (!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
}

QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // Draw the line itself
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);

    // Draw the arrows
    double angle = std::atan2(-line.dy(), line.dx());

    QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                                  cos(angle + M_PI / 3) * arrowSize);
    QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                                  cos(angle + M_PI - M_PI / 3) * arrowSize);
    QPointF destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                              cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                              cos(angle - M_PI + M_PI / 3) * arrowSize);

    painter->setBrush(Qt::black);
    painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
}
