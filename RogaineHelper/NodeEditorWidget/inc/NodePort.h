#pragma once

#include <QtWidgets>

class NodeItem;
class NodeConnection;



class NodePort : public QGraphicsItem
{
public:
	enum { Type = UserType + 666 + 6 };
    int type() const { return Type;}

    enum PortType                                       { In = 1, Out = 2, InOut = 3 };
    PortType portType() const                           { return mPortType; }
    void setPortType(PortType c)                        { mPortType = c;	}

    enum PortAlignment                                  { None = 0, Left = 1, Right = 2, Bottom = 3, Top = 4 };
    PortAlignment portAlignment() const                 { return mPortAlignment;	}
    void setPortAlignment(PortAlignment alignment)      { mPortAlignment = alignment;}

    bool singleConnection() const                       { return mSingleConnection; }
    void setSingleConnection(bool singleConnection)     { mSingleConnection = singleConnection;	}


    NodePort(NodeItem *parentItem,
             PortType portType = PortType::In,
             const PortAlignment portAlignment = PortAlignment::Left,
             const bool singleConnection = false);

    virtual ~NodePort();



    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void updatePosition();
    void addConnection(NodeConnection *connection);
    void deleteConnection(NodeConnection *connection);
	void deleteConnections();
    void removeConnection(NodeConnection *connection);

    void setHighlight(bool highlight);

//private:
	int mRadius;
    PortType mPortType;
    NodeItem *ownerItem;
	QWidget* mWidget;
    PortAlignment mPortAlignment;
    QList<NodeConnection*> connections;

	QColor darkColor;
	bool highlight;
    bool mSingleConnection;
	bool mDisableWidgetOnConnection;

    //ugly, but node ctor is called after wiget is removed, but delete of port would otherwise activate label widget
    void removeWidget();

	void update(const QRectF & rect = QRectF());

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	void debugPaint(QPainter *painter);

};

