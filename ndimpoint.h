#ifndef NDIMPOINT_H
#define NDIMPOINT_H

#include <QWidget>
#include <QGraphicsItem>
class Edge;
class NDimPoint {
private:
    quint32 _dims;
    QVector<double> _points, _originalPoints;
public:
    explicit NDimPoint(quint16 dims = 2, QVector<double> point = {0,0});
    void project(quint16 dim, QVector<double> &inputs, QVector<double> &retVal) const;
    void rotate(quint16 d1, quint16 d2, double theta);
    void setPoints(QVector<double> &&points);
    void resetPoints();
    const QVector<double>& getPoints() const;
};

class NDimNode : public QGraphicsItem
{
public:
    explicit NDimNode(quint32 pointNum, QGraphicsItem* parent = nullptr, quint16 dims = 2, QVector<double> point = {0,0});

    void resetPosition();
    //Handle projecting if it needs to be done, otherwise do nothing
    void project(QVector<double> &tempInput, QVector<double> &tempOutput, const QVector<double>& xyOffsets);
    void rotate(quint16 d1, quint16 d2, double theta);
    void setPoints(QVector<double> &&points);

    void addEdge(Edge*);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    QPainterPath shape() const override;
    quint32 pointNum() const;
    quint32 dims() const;

private:
    //Project from dim to dim-1
    QSharedPointer<NDimPoint> _point;
    quint32 _pointNum, _dims;
    QVector<Edge*> _edges;
    mutable double _actualX, _actualY;
};

class Edge : public QGraphicsItem
{
public:
    Edge(NDimNode *sourceNode, NDimNode *destNode);

    NDimNode *sourceNode() const;
    NDimNode *destNode() const;

    void adjust();

    enum { Type = UserType + 2 };
    int type() const override { return Type; }
    void setDotted(bool dotted);
    QColor fillColor;
    qint8 cidx;

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    NDimNode *source, *dest;
    QPointF sourcePoint;
    QPointF destPoint;
    qreal arrowSize;
    bool _dotted;
};

#endif
