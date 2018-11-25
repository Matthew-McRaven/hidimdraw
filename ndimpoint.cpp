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
// Distance from the camera to the center of the object
const int cameraDist = 600;
// Distance of the display surface from the camera
const int viewerDist = 50;
NDimNode::NDimNode(quint32 pointNum, QGraphicsItem *parent, quint16 dims, QVector<double> point):
    QGraphicsItem(parent), _pointNum(pointNum), _dims(dims), _actualX(std::numeric_limits<double>::quiet_NaN()),
    _point(new NDimPoint(dims, point))

{
    setZValue(0);
}

void NDimNode::resetPosition()
{
    _point->resetPoints();
}

void NDimNode::project(QVector<double> &tempInput, QVector<double> &tempOutput, const QVector<double> &xyOffsets)
{
    // Perspective projections only make sense from dimensions > 2.
    if(_dims<2) {
        return;
    }
    // Input to NDimPoint::project(...) is a list containing
    _point->project(tempInput , tempOutput);
    _actualX = tempOutput[0] + xyOffsets[0];
    _actualY = tempOutput[1] + xyOffsets[1];
    this->setPos(QPointF(_actualX, _actualY));
    for(int it=0; it<_edges.size();it++) _edges[it]->adjust();
}

QPainterPath NDimNode::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void NDimNode::rotate(quint16 d1, quint16 d2, double theta)
{
    //Remove cache (X, Y) values
    _actualX = std::numeric_limits<float>::quiet_NaN();
    _point->rotate(d1, d2, theta);
}

void NDimNode::setPoints(QVector<double> &&points)
{
    _actualX = std::numeric_limits<float>::quiet_NaN();
    if(points.size() != _dims) throw -1;
    _point->setPoints(std::move(points));
}

void NDimNode::addEdge(Edge * edge)
{
    _edges << edge;
    edge->adjust();
}

QRectF NDimNode::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

void NDimNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::NoPen);
    QColor color;
    if(_pointNum == 0) color = QColor("red");
    else if(_pointNum == (1<<_dims)-1) color = QColor("green");
    else if(std::bitset<sizeof(quint32)>(_pointNum).count()%2==1)color=QColor("blue");
    else color = QColor("orange");
    painter->setBrush(color);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-5,-5,10,10);
}

Edge::Edge(NDimNode *sourceNode, NDimNode *destNode)
    : arrowSize(10), _dotted(false)
{
    setAcceptedMouseButtons(0);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
    setZValue(-1);
    adjust();
}

NDimNode *Edge::sourceNode() const
{
    return source;
}

NDimNode *Edge::destNode() const
{
    return dest;
}

void Edge::adjust()
{

    if (source==nullptr || dest==nullptr) return;

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

void Edge::setDotted(bool dotted)
{
    _dotted = dotted;
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
    painter->setPen(QPen(fillColor, 1, _dotted ? Qt::DotLine : Qt::SolidLine,
                         Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);

    // Draw the arrows
    painter->setPen(QPen(fillColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    double angle = std::atan2(-line.dy(), line.dx());
    QPointF offset(arrowSize*sin(angle+M_PI/2)/2, arrowSize*cos(angle+M_PI/2)/2);
    QPointF destArrowP1 = (sourcePoint+destPoint)/2 + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                                  cos(angle + M_PI / 3) * arrowSize);
    QPointF destArrowP2 = (sourcePoint+destPoint)/2 + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                                  cos(angle + M_PI - M_PI / 3) * arrowSize);

    painter->setBrush(fillColor);
    painter->drawPolygon(QPolygonF() << (line.p1()+line.p2()-2*offset)/2 << destArrowP1-offset << destArrowP2-offset);
}



void NDimPoint::projectHelper(quint16 dim, QVector<double> &inputs, QVector<double> &retVal) const
{
    //Perspective projection makes no sense in less than 2 dimensions, so abort.
    if(dim < 2) return;
    // While this might make more logical sense as a recurssive algorithm, perofrmance testing
    // showed the perspective projection to be the slowest part of the program.
    // By reducing the number of stack allocations, and using a fixed amount of memory, performance was greatly increased.
    // For dimensions from dim to 2, and for points in that dimension
    for(int dimIt = dim; 2 <= dimIt ; dimIt--) {
        for(int it = 0; it < dimIt; it++) {
            // Perform a pespective projection from a dimension of dimIt to dimIt-1.
            // Uses the calculation described here:
            // https://en.wikipedia.org/wiki/3D_projection#Perspective_projection
            // Assumes that the camera is orthogonal to the
            inputs[it] = inputs[it] * cameraDist / (cameraDist + viewerDist + inputs[dimIt]);
        }
    }
    for(int it = 0; it < 2; it++) {
        retVal[it] = inputs[it];
    }
}

void NDimPoint::project(QVector<double> &inputs, QVector<double> &retVal) const
{
    for(int it=0; it < getPoints().size(); it++) inputs[it] = _points[it];
}

void NDimPoint::rotate(quint16 d1, quint16 d2, double theta)
{
    if(theta == 0) return;
    else {
        //Now perform rotation
        double a = _points[d1], b = _points[d2];
        double nT = theta;
        _points[d1] = a*cos(nT) - b * sin(nT);
        _points[d2] = a*sin(nT) + b * cos(nT);
    }
}

const QVector<double> &NDimPoint::getPoints() const
{
    return _points;
}

void NDimPoint::setPoints(QVector<double> &&points)
{
    _originalPoints = std::move(points);
    resetPoints();

}

void NDimPoint::resetPoints()
{
    for(int it=0; it<_originalPoints.size(); it++) _points[it] = _originalPoints[it];
}
