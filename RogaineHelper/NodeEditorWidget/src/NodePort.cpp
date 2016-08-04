#include <QtWidgets>

#include "NodeItem.h"
#include "NodeConnection.h"
#include "NodePort.h"

NodePort::NodePort(NodeItem *parentItem,
                   NodePort::PortType portType,
                   const PortAlignment connectorAlignment,
                   bool singleConnection)
        : QGraphicsItem(parentItem),
          ownerItem(parentItem),
          mWidget(parentItem->widget()),
          mPortType(portType),
          mPortAlignment(connectorAlignment),
          mSingleConnection(singleConnection),
          mRadius(7)
{
    setCacheMode(DeviceCoordinateCache);
	setZValue(2);
	updatePosition();
	//setFlag(QGraphicsItem::ItemIsSelectable, true);

    if (portType == PortType::In) {
		darkColor =  QColor(Qt::darkYellow);
	}
    else if (portType == PortType::Out) {
		darkColor =  QColor(Qt::darkRed).light(80);
	}
	else {
		darkColor =  QColor(Qt::darkGreen);
	}
	highlight = false;
}



NodePort::~NodePort()
{
    this->deleteConnections();
    //dw super class destructor should do this, right?
    if (scene() != NULL) {
        this->scene()->removeItem(this);
    }
}



void NodePort::updatePosition() {
	this->prepareGeometryChange();

	//if we dont have parent, stay where we are
    if (ownerItem == NULL || mWidget == NULL) {
		return;
	}

    QPointF pPos = ownerItem->pos();
	QSize widgetSize = mWidget->size();
	//dw677:
	//QSizeF widgetSize = parent->size();
	//radius = labelSize.height()/2;

	QPointF newPos;
    if (portAlignment() == NodePort::Left) {
		newPos.setX(-mRadius);
		//newPos.setY(mWidget->pos().y() + widgetSize.height()/2.0);
		//dw677:
		//newPos.setY(parent->pos().y() + widgetSize.height()/2.0);
		//newPos.setY(mWidget->pos().y() + mWidget->rect().height()/2.0);
		//newPos.setY(mWidget->pos().y() + mWidget->rect().height()/2.0);
		//newPos.setY(mWidget->pos().y() + mWidget->height()/2.0);
        newPos.setY(ownerItem->subWidgetRect(mWidget).y() + ownerItem->subWidgetRect(mWidget).height()/2.0);
	}
    else if (portAlignment() == NodePort::Right) {
		//seems to be to big
		//newPos.setX(parent->boundingRect().width()+mRadius+1);
		//seems to be size of content??? too small
		//newPos.setX(parent->window()->size().width()+mRadius+1);
		//float rightx = dynamic_cast<NodeItem*>(parent)->widget()->width()
		//	+ mRadius + 1;
		//float rightx = dynamic_cast<NodeItem*>(parent)->widget()->geometry().size().width()
		//	+ mRadius + 1;
		/*
		float rightx = dynamic_cast<NodeItem*>(parent)->widget()->width()
			+ dynamic_cast<NodeItem*>(parent)->widget()->baseSize().width()
			+ mRadius + 1;
			*/

		//dw FIXME: ugly
        newPos.setX(ownerItem->rect().width() + mRadius);
		//newPos.setY(mWidget->pos().y()+ widgetSize.height()/2.0);
        newPos.setY(ownerItem->subWidgetRect(mWidget).y()+ ownerItem->subWidgetRect(mWidget).height()/2.0);
	}
    else if (portAlignment() == NodePort::Bottom) {
		//newPos.setX(mWidget->pos().x() + widgetSize.width()/2.0);
        newPos.setX(ownerItem->subWidgetRect(mWidget).x() + ownerItem->subWidgetRect(mWidget).width()/2.0);

		//newPos.setY(parent->widget()->rect().height() + mRadius);
		//newPos.setY(parent->boundingRect().height() + mRadius);
        newPos.setY(ownerItem->rect().height() + mRadius);
	}
    else if (portAlignment() == NodePort::Top) {
		//newPos.setX(mWidget->pos().x() + widgetSize.width()/2.0);
        newPos.setX(ownerItem->subWidgetRect(mWidget).x() + ownerItem->subWidgetRect(mWidget).width()/2.0);
		newPos.setY(-mRadius);
	}


	this->setPos(newPos);
	//dw seems to always paint -> is already called from update? or will cause updatePos in next frame again?
	//update();

    foreach (NodeConnection *c, this->connections) {
		//dw problem: label already deleted but connector tries to enable it?
        //c->paint(painter, option, w);
		c->updatePosition();
    }
	update();
}


void NodePort::setHighlight(bool highlight) {
	prepareGeometryChange();
	this->highlight = highlight;
	this->updatePosition();
	this->update();
}

QRectF NodePort::boundingRect() const
{
    qreal adjust = 1;
    return QRectF(-mRadius - adjust, -mRadius - adjust, 2*(mRadius + adjust), 2*(mRadius + adjust));
	//return QGraphicsItem::boundingRect();
}

QPainterPath NodePort::shape() const
{
    QPainterPath path;
    path.addEllipse(-mRadius, -mRadius, 2*mRadius, 2*mRadius);
    return path;
}


void NodePort::debugPaint(QPainter *painter)
{
	static int i = 0, j=0, k=0;
    painter->fillRect(boundingRect(), QColor(i=(i+19)%256 , j=(j+51)%256, k=(k+11)%256)); // to see item.
}


void NodePort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* w)
{
	Q_UNUSED(option);
    Q_UNUSED(w);

	//dw 699:
	painter->setRenderHint(QPainter::Antialiasing);

    // TODO debug draw
    //if (static_cast<NodeEditorScene*>(scene())->settings.debugDraw) {
		debugPaint(painter);
    //}

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
	//painter->drawEllipse(-mRadius*0.8, -mRadius*0.8, mRadius, mRadius);
	//painter->drawEllipse(0, 0, 2*mRadius, 2*mRadius);
    QRadialGradient gradient(-mRadius/2, -mRadius/2, mRadius);
	if (/*option->state & QStyle::State_Sunken*/ highlight) {
        gradient.setCenter(mRadius/2, mRadius/2);
        gradient.setFocalPoint(mRadius/2, mRadius/2);
        //gradient.setColorAt(1, QColor(Qt::yellow).light(120));
        //gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
		gradient.setColorAt(1, darkColor.light(240));
		gradient.setColorAt(0, darkColor.light(120));
    } else {
        //gradient.setColorAt(0, Qt::yellow);
        //gradient.setColorAt(1, Qt::darkYellow);
		gradient.setColorAt(0, darkColor.light(240));
        gradient.setColorAt(1, darkColor);
    }
	painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    //painter->drawEllipse(-mRadius, -mRadius, mRadius, mRadius);
	painter->drawEllipse(-mRadius, -mRadius, 2*mRadius, 2*mRadius);

	//dw new2, is abstract
	//QGraphicsItem::paint(painter, option, w);
	
	/*
	//dw hacky, good idea??
	NodeConnection* a;
	a->p
	foreach(NodeConnection* a, arrows) {
		a->paint(painter, option, NULL);
	}
*/
	
/*
	painter->drawEllipse(-7, -7, 20, 20);
    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
    } else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
	*/
}


void NodePort::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << __FUNCTION__;

    update();
    QGraphicsItem::mousePressEvent(event);
}

void NodePort::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << __FUNCTION__;

    setHighlight(false);
    QGraphicsItem::mouseReleaseEvent(event);
}


void NodePort::addConnection(NodeConnection *connection)
{
    if (mSingleConnection && connections.count() > 0) {
		//delete arrows.first();
		//arrows.clear();
		deleteConnections();
	}
    connections.append(connection);
}

void NodePort::deleteConnection(NodeConnection *connection)
{
	//TODO: how to handle deletion exactly, when to emit signals
	//arrow->startConnector()->arrows.removeOne(arrow);
	//arrow->endConnector()->arrows.removeOne(arrow);

	//deleting arrow will also remove it from this connector
	//dw new
	//arrow->setParentItem(NULL);
    delete connection;
	//arrows removed during connection destruction, but attached outside => bad idea
}

/* removes a connection, but does not delete it*/
void NodePort::removeConnection(NodeConnection *connection)
{
    connections.removeOne(connection);
}

void NodePort::deleteConnections()
{
    foreach(NodeConnection* arrow, connections) {
		deleteConnection(arrow);
	}
}

void NodePort::removeWidget()
{
	mWidget = NULL;
    ownerItem = NULL;
}


void NodePort::update(const QRectF & rect) {
	
	//dw new3: why do ourself
    foreach (NodeConnection *c, this->connections) {
		//dw problem: label already deleted but connector tries to enable it?
        //c->paint(painter, option, w);
		c->update(rect);
    }
	
	QGraphicsItem::update(rect);
}

















/*

//dw667 backmerge: was active
void NodeConnector::updatePositionGeometry() {
    //prepareGeometryChange();
    updatePosition();
    foreach (NodeConnection *c, this->arrows) {
        //dw problem: label already deleted but connector tries to enable it?
        //c->paint(painter, option, w);
        c->updatePositionGeometry();
    }
    update();
}

QVariant NodePort::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (Edge *edge, edgeList)
            edge->adjust();
        graph->itemMoved();
        break;
    default:
        break;
    };
    return QGraphicsItem::itemChange(change, value);
}

//not called, does the event exist for this class???
void NodePort::mouseOverEvent(QGraphicsSceneMouseEvent *event)
{
    highlight = true;
    update();
    //QGraphicsItem::mouseOverEvent(event);
}
void NodePort::enterEvent ( QEvent * event )  {
    highlight = true;
    //QGraphicsItem::enterEvent(event);
}

void NodePort::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

QList<Edge *> NodePort::edges() const
{
    return edgeList;
}

void NodePort::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this) {
        newPos = pos();
        return;
    }

    // Sum up all forces pushing this item away
    qreal xvel = 0;
    qreal yvel = 0;
    foreach (QGraphicsItem *item, scene()->items()) {
        NodeConnector *node = qgraphicsitem_cast<NodeConnector *>(item);
        if (!node)
            continue;

        QLineF line(mapFromItem(node, 0, 0), QPointF(0, 0));
        qreal dx = line.dx();
        qreal dy = line.dy();
        double l = 2.0 * (dx * dx + dy * dy);
        if (l > 0) {
            xvel += (dx * 150.0) / l;
            yvel += (dy * 150.0) / l;
        }
    }

    // Now subtract all forces pulling items together
    double weight = (edgeList.size() + 1) * 10;
    foreach (Edge *edge, edgeList) {
        QPointF pos;
        if (edge->sourceNodeConnector() == this)
            pos = mapFromItem(edge->destNodeConnector(), 0, 0);
        else
            pos = mapFromItem(edge->sourceNodeConnector(), 0, 0);
        xvel += pos.x() / weight;
        yvel += pos.y() / weight;
    }

    if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
        xvel = yvel = 0;

    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
}

bool NodePort::advance()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}*/
