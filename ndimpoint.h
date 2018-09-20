#ifndef NDIMPOINT_H
#define NDIMPOINT_H

#include <QWidget>
#include <QGraphicsItem>
class Edge;
class NDimPoint : public QGraphicsItem
{
public:
    NDimPoint(quint32 pointNum, QGraphicsItem* parent = nullptr, quint16 dims = 2, QVector<double> point = {0,0});

    void rotate(quint16 d1, quint16 d2, double theta);
    void setPoints(QVector<double> && points);
    void addEdge(Edge*);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    QPainterPath shape() const override;
    quint32 pointNum() const;
    quint32 dims() const;

    //Handle projecting if it needs to be done, otherwise do nothing
    void project();

private:
    //Project from dim to dim-1
    void project(quint16 dim, QVector<double> inputs, QVector<double>& retVal) const;
    quint32 _pointNum, _dims;
    QVector<double> _points;
    QVector<Edge*> _edges;
    mutable double _actualX, _actualY;
};

class Edge : public QGraphicsItem
{
public:
    Edge(NDimPoint *sourceNode, NDimPoint *destNode);

    NDimPoint *sourceNode() const;
    NDimPoint *destNode() const;

    void adjust();

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    NDimPoint *source, *dest;

    QPointF sourcePoint;
    QPointF destPoint;
    qreal arrowSize;
};

#endif
