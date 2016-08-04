#pragma once

#include <QtWidgets>

class NodeItem;
class NodePort;


class NodeConnection : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 666 + 0 };
    int type() const { return Type;}

	virtual ~NodeConnection();
    NodeConnection(NodePort *startPort,
                   NodePort *endPort,
                   QGraphicsItem *parent = 0,
                   QGraphicsScene *scene = 0,
                   bool bidirectional = true);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setColor(const QColor &color)  { mColor = color; }
    NodePort *startPort() const         { return mStartPort; }
    NodePort *endPort() const           { return mEndPort; }

	void updatePosition();

	bool bidirectional();
	bool setBidirectional(bool bidirectional);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	//returns used/created control points (eg for debug draw,...)
	void recreatePath(QPointF& controlPoint1, QPointF& controlPoint2);
    QPolygonF createArrowPoly(QPainterPath& p, NodePort* conn);

private:
    NodePort *mStartPort;
    NodePort *mEndPort;
    QColor mColor;
    
	qreal arrowSize;
	bool mBidirectional;

	void debugPaint(QPainter *painter, QPointF& controlPoint1, QPointF& controlPoint2);


};

