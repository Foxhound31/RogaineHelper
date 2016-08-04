#include <QtWidgets>
#include <QStyleFactory>
#include "NodeItem.h"
#include "NodeConnection.h"
#include "NodePort.h"



NodeItem::NodeItem(QWidget * widget, Qt::WindowFlags wFlags, QGraphicsItem *parent) :
    QGraphicsProxyWidget(parent, wFlags),
    isMoving(false),
    isMovable(true),
    isResizable(false)
{
//    setCacheMode(DeviceCoordinateCache);
//    setZValue(1);
//    setFlag(QGraphicsItem::ItemIsMovable, true);
//    setFlag(QGraphicsItem::ItemIsSelectable, true);
//    setFlag(QGraphicsItem::ItemIsFocusable, true);

//    QDialog * itemDialog = new QDialog;
//    QVBoxLayout * layout = new QVBoxLayout(itemDialog);
//    itemDialog->setLayout(layout);
//    widget->setFixedSize(widget->geometry().size());
//    ExpanderWidget* ew = new ExpanderWidget(itemDialog);
//    ew->setExpanderTitle("Filter settings");
//    ExpanderWidget* ew2 = new ExpanderWidget(itemDialog);
//    ew2->setExpanderTitle("Ports");
//    if (widget) {
//        layout->addWidget(ew);
//        layout->addWidget(ew2);
//        layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
//        ew->addPage(widget);
//        ew2->addPage(new QLabel("ports here", ew2));
//        ew->show();
//        ew2->show();
//        widget->show();
//    }

//    itemDialog->adjustSize();

//    QHBoxLayout * layout = new QHBoxLayout;
//    layout->setSizeConstraint(QLayout::SetFixedSize);
//    if (widget) {
//        widget->setFixedSize(widget->geometry().size());
//        layout->addWidget(widget);
//        widget->setWindowFlags(Qt::Dialog);
//        widget->show();
//    }



//    itemDialog->setLayout(layout);
//    itemDialog->setWindowFlags(Qt::Dialog);
//    itemDialog->setVisible(true);
//    itemDialog->update();
//    itemDialog->adjustSize();

//    setWidget(itemDialog);


    //widget->setFixedSize(widget->geometry().size());

//    setWidget(widget);

//    setWindowFlags(Qt::Dialog);
//    setVisible(true);
//    update();
//    widget->adjustSize();


}



NodeItem::~NodeItem()
{
    //removeWigetFromPorts();
    //deleteConnections();

    QWidget * embeddedWidget = widget();
    setWidget(0);
    delete embeddedWidget;
}

QRectF NodeItem::boundingRect() const
{
    QRectF rect(QGraphicsProxyWidget::boundingRect());
    qreal extra = 3;
    rect = rect.normalized().adjusted(-extra, 0, extra, 0);
    //rect.setWidth(rect.width() - 2 * (mMaxRadius + 1));
    //rect.setX(rect.x() + (mMaxRadius + 1));
    //rect.setWidth(rect.width() - 4);
    //rect.setX(rect.x() + 2);
    return rect;
}

QPainterPath NodeItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}



void NodeItem::addPort(NodePort* port)
{
    ports.append(port);
    updatePortsPos();
	prepareGeometryChange();
}


void NodeItem::deleteConnections()
{
    foreach (NodePort *port, ports) {
		//dw problem: label already deleted but connector tries to enable it?
        port->deleteConnections();
    }

	//dw good location?, needed?
	//this->scene()->removeItem(this);
}


void NodeItem::removeWidgetFromPorts()
{
    foreach (NodePort *c, ports) {
		//dw problem: label already deleted but connector tries to enable it?
        c->removeWidget();
    }
}


void NodeItem::updatePortsPos() {
    foreach (NodePort *port, ports) {
        port->updatePosition();
    }
}


void NodeItem::debugPaint(QPainter *painter)
{
	static int i = 0, j=0, k=0;
    painter->fillRect(boundingRect(), /*Qt::green*/ QColor(i=(i+19)%256 , j=(j+51)%256, k=(k+11)%256));
}



//void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w)
//{
//    debugPaint(painter);
//	QGraphicsProxyWidget::paint(painter, option, w);
//}


bool NodeItem::shouldMoveNode(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointer<QWidget> hitWidget = widget()->childAt(mouseEvent->pos().toPoint());
    if ( (hitWidget == NULL) ||
         (hitWidget->inherits("QDialog"))  ||
         (hitWidget->inherits("QFrame"))   ||
         (hitWidget->inherits("QGroupBox")))  {
            return true;
    }
    return false;
}


QVariant NodeItem::itemChange(GraphicsItemChange change, //dw FIXME: needs much cleanup
                     const QVariant &value)
{
   if (change == QGraphicsItem::ItemSelectedChange ||
       change == QGraphicsItem::ItemTransformChange ||
       change == QGraphicsItem::ItemScaleChange ||
       change == QGraphicsItem::ItemSendsGeometryChanges ||
       change == QGraphicsItem::ItemMatrixChange ||
       change == QGraphicsItem::ItemPositionHasChanged ||
       change == QGraphicsItem::ItemPositionChange ) {
           updatePortsPos();
   }

    return QGraphicsProxyWidget::itemChange(change, value);
}






void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << __FUNCTION__;

    if (shouldMoveNode(mouseEvent)) {
        QGraphicsItem::mousePressEvent(mouseEvent);
        isMoving = true;
        scene()->clearSelection();
        scene()->clearFocus();
        setSelected(true);
    }
    else {
        QGraphicsProxyWidget::mousePressEvent(mouseEvent);
    }
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    qDebug() << __FUNCTION__;
    QGraphicsProxyWidget::mouseMoveEvent(mouseEvent);

    if (isMoving) {
		QGraphicsItem::mouseMoveEvent(mouseEvent);

		//dw667 backmerge: was active
		//dw new5
        //foreach (NodeConnector *con, connectors) {
        //	//dw667 backmerge: was active
        //	//con->updatePositionGeometry();
        //	//dw667 backmerge: new
        //	con->updatePosition();
        //}
	}

	//scene()->clearSelection();
	//setSelected(true);
    //updateConnectorsPos();

}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << __FUNCTION__;

	if (isMoving) {
		isMoving = false;
	}

	//call both always to lose no events and to not have to do the same shit for clicks too
    QGraphicsItem::mouseReleaseEvent(mouseEvent);
    QGraphicsProxyWidget::mouseReleaseEvent(mouseEvent);
	scene()->clearSelection();
	setSelected(true);
    //updateConnectorsPos();
}




