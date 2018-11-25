#ifndef NDIMPOINT_H
#define NDIMPOINT_H

#include <QWidget>
#include <QGraphicsItem>
class Edge;
/*
 * NDimPoint is a N-dimensional point class, used to encapsulate the perspective projection and
 * rotation calculations.
 */
class NDimPoint {
private:
    //Number of dimensions in which the point exists.
    //The minimum useful dimension is 1.
    quint32 _dims;
    //Maintain a list of original points, so that resetting is trivial.
    QVector<double> _points, _originalPoints;

    // The input parameter is a vector of length _dims,
    // which contains the (x, y, z, ...) coordinates of the point in _dims dimensional space.
    // Since the intemediate results of the projection are useless, inputs is used as a scratch buffer in memory
    // for the iterative projections from dimension of dim to 2.
    void projectHelper(quint16 dim, QVector<double> &inputs, QVector<double> &retVal) const;
public:
    NDimPoint(quint16 dims = 2, QVector<double> point = {0,0});
    // Use a perspective projection to project the point FROM dimension of dim to a dimension of 2.
    // The resulting projections (x,y) values are placed in retVal.
    // Inputs is a vecotr of length _dims that is used for intermediary calculations
    void project(QVector<double> &inputs, QVector<double> &retVal) const;
    // Rotate the point in the (d1,d2) plane by theta radians.
    void rotate(quint16 d1, quint16 d2, double theta);
    void setPoints(QVector<double> &&points);
    void resetPoints();
    const QVector<double>& getPoints() const;
};
/*
 * Multi-Dimensional Graph View Node class
 */
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

/*
 *
 */
class Edge : public QGraphicsItem
{
public:
    Edge(NDimNode *sourceNode, NDimNode *destNode);

    NDimNode *sourceNode() const;
    NDimNode *destNode() const;

    // Must manually redraw edge anytime the source or dest moves
    void adjust();

    enum { Type = UserType + 2 };
    int type() const override { return Type; }
    void setDotted(bool dotted);
    QColor fillColor;

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    //Non-owning pointer to nodes
    NDimNode *source, *dest;
    QPointF sourcePoint, destPoint;
    qreal arrowSize;
    bool _dotted;
};

#endif
