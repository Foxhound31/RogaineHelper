#include <QtGui>
#include <QtWidgets>
#include <QGraphicsSceneEvent>
#include <QDebug>

#include "NodeEditorScene.h"
#include "NodeItem.h"
#include "NodeConnection.h"
#include "NodePort.h"




// move node with all connected lines
QVariant CustomRect::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newCenterPos = QPointF(value.toPointF().x() + rect().center().x(), value.toPointF().y() + rect().center().y());
        // Move the required point of the line to the center of the elipse
        if (_nextLine)
            _nextLine->setLine(QLineF(newCenterPos, _nextLine->line().p2()));
        if (_prevLine)
            _prevLine->setLine(QLineF(_prevLine->line().p1(), newCenterPos));
    }
    return QGraphicsItem::itemChange(change, value);
}


// get total length of all connected lines
int CustomRect::getTotalLen()
{
    int result = 0;
    CustomRect * tmpItem;

    tmpItem = this;
    while(tmpItem) {
        if (tmpItem->nextLine()) {
            result += tmpItem->nextLine()->line().length();
            tmpItem = tmpItem->nextLine()->nextRect();
        }
        else break;
    }

    tmpItem = this;
    while(tmpItem) {
        if (tmpItem->prevLine()) {
            result += tmpItem->prevLine()->line().length();
            tmpItem = tmpItem->prevLine()->prevRect();
        }
        else break;
    }
    return result;
}


// get total length of all connected lines
int CustomLine::getTotalLen() {
    if (_nextRect)
        return _nextRect->getTotalLen();
    else if (_prevRect)
        return _prevRect->getTotalLen();
    else return 0;
}
















NodeEditorScene::NodeEditorScene(QWidget *parent)
    : QGraphicsScene(parent)
{
    tmpConnection = nullptr;
    tmpPort = nullptr;
    lastHighlighted = nullptr;
    conn = nullptr;
    isLineDrawing = false;
    clickMode = CLICKMODE_SELECT;

    setStickyFocus(true); // clicking into the scene background will clear focus
}


void NodeEditorScene::deleteSelectedItems()
{
    QList<QGraphicsItem*> itemsToDelete = selectedItems();

    if (itemsToDelete.isEmpty()) {
        qDebug() << "No items selected";
        return;
    }

    foreach(QGraphicsItem* item, itemsToDelete)
    {
        if (item->type() != QGraphicsEllipseItem::Type) { // dont remove circle marks
            removeItem(item);
            delete item;
        }
    }
}




void NodeEditorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void NodeEditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    const QPointF center = mouseEvent->scenePos();
    QGraphicsItem * activeItem = itemAt(center, QTransform());

    if (isLineDrawing) {
        currentLine->setLine(currentLine->line().x1(),currentLine->line().y1(), center.x(), center.y());
    }

    //if (activeItem->type() == QGraphicsLineItem::Type) {
    //
    //}

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void NodeEditorScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    const QPointF center = mouseEvent->scenePos();
    QGraphicsItem * activeItem = itemAt(center, QTransform());

    if (activeItem->type() == QGraphicsLineItem::Type ||
        activeItem->type() == QGraphicsRectItem::Type ||
        activeItem->type() == CustomLine::Type ||
        activeItem->type() == CustomRect::Type ) {

        emit itemSelected(activeItem);
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void NodeEditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();

    // right button stop line drawing
    if (mouseEvent->button() == Qt::RightButton) {
        if (isLineDrawing) {
            lastItem->setNextLine(nullptr);
            isLineDrawing = false;
            removeItem(currentLine);
            delete currentLine;
        }
    }

    // left button to add item
    else if (mouseEvent->button() == Qt::LeftButton) {

        if (clickMode == CLICKMODE_ADD_CIRCLE) {
            int radius = 20;

            const QPointF center = mouseEvent->scenePos();
            QGraphicsItem * activeItem = itemAt(center, QTransform());

            // Draw circle
            QGraphicsEllipseItem* newItem = new QGraphicsEllipseItem(center.x()-radius, center.y()-radius, 2*radius, 2*radius);
            //newItem->setFlag(QGraphicsItem::ItemIsMovable, true);
            newItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
            newItem->setFlag(QGraphicsItem::ItemIsFocusable, true);
            newItem->setZValue(1);

            QPen newPen(Qt::blue);
            newPen.setWidth(5);
            newItem->setPen(newPen);

            addItem(newItem);

            emit itemInserted(newItem);
        }

        if (clickMode == CLICKMODE_ADD_RECT) {
            int radius = 7;
            const QPointF center = mouseEvent->scenePos();

//            // Draw new line
//            QGraphicsLineItem* newLine = new QGraphicsLineItem(center.x(), center.y(), center.x(), center.y());
//            //newLine->setFlag(QGraphicsItem::ItemIsMovable, true);
//            newLine->setFlag(QGraphicsItem::ItemIsSelectable, true);
//            QPen linePen(Qt::black);
//            linePen.setWidth(5);
//            newLine->setPen(linePen);
//            addItem(newLine);


            // Draw new line
            CustomLine * newLine = new CustomLine(QLineF(center.x(), center.y(), center.x(), center.y()));
            //newLine->setFlag(QGraphicsItem::ItemIsMovable, true);
            newLine->setFlag(QGraphicsItem::ItemIsSelectable, true);
            QPen linePen(Qt::black);
            linePen.setWidth(5);
            newLine->setPen(linePen);
            addItem(newLine);

            // Draw new point
            CustomRect * newItem = new CustomRect(QRectF(center.x()-radius, center.y()-radius, 2*radius, 2*radius));
            QPen newPen(Qt::black);
            newPen.setWidth(1);
            newItem->setPen(newPen);
            QBrush newBrush(Qt::black);
            newItem->setBrush(newBrush);
            addItem(newItem);

            // Connect previous line to the hew item
            if (isLineDrawing && currentLine) {
                currentLine->setNextRect(newItem);
                newItem->setPrevLine(currentLine);
            }
            // Connect next line
            newItem->setNextLine(newLine);
            newLine->setPrevRect(newItem);

            currentLine = newLine;
            lastItem = newItem;
            isLineDrawing = true;
        }

    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}








/*
void NodeEditorScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << __FUNCTION__;

    if (mouseEvent->button() != Qt::LeftButton)
        return;

    QList<QGraphicsItem *> startConnectors = items(mouseEvent->scenePos());
    foreach(QGraphicsItem * g, startConnectors) {
        if (g->type() == NodePort::Type) {
            NodePort* start = dynamic_cast<NodePort*>(g);
            //dw ugly
            if (start->mSingleConnection) {
                start->deleteConnections();
            }

            tmpPort = new NodePort(nullptr, this, nullptr, NodePort::InOut);
            tmpPort->setPos(mouseEvent->scenePos());
            //dw needed? addItem(tmpConnector);

            // TODO debug draw
            //if (settings.debugDraw) {
                tmpPort->mRadius = 5;
            //}
            //else {
            //    tmpPort->setVisible(false);
            //    tmpPort->mRadius = 0;
            //}


            if (start->portType() != NodePort::In) {
                tmpConnection = new NodeConnection(start, tmpPort, NULL, this);
            }
            else {
                tmpConnection = new NodeConnection(tmpPort, start, NULL, this);
            }
            //dw needed? addItem(tmpArrow);
            start->setHighlight(true);
            existingPort = start;


            existingPort->updatePosition();
            existingPort->update();
            tmpPort->updatePosition();
            tmpPort->update();
            tmpConnection->updatePosition();
            tmpConnection->update();

            //dw667 backmerge: was commented
            //QGraphicsScene::mousePressEvent(mouseEvent);
            return;
            //addItem(tmpArrow);
            //if (
        }
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}


void NodeEditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << __FUNCTION__;

    if (tmpConnection != 0) {

		//simple right/left handling
        if (tmpConnection->startPort()->mapToScene(0,0).x() < tmpConnection->endPort()->mapToScene(0,0).x()) {
            if (tmpPort == tmpConnection->endPort())
                tmpPort->setPortAlignment(NodePort::PortAlignment::Left);// right = false;
			else
				//tmpConnector->right = true;
                tmpPort->setPortAlignment(NodePort::PortAlignment::Right);
		}
		else {
            if (tmpPort == tmpConnection->endPort())
				//tmpConnector->right = true;
                tmpPort->setPortAlignment(NodePort::PortAlignment::Right);
			else
				//tmpConnector->right = false;
                tmpPort->setPortAlignment(NodePort::PortAlignment::Left);
		}

		// if we hit a connecor, highlight it and take 'right' over, switch in and out if neccessary
		QList<QGraphicsItem *> intersectedItems = items(mouseEvent->scenePos());
        while(intersectedItems.count() && (intersectedItems.first() == tmpConnection || intersectedItems.first() == tmpPort || intersectedItems.first()->type() != NodePort::Type)) {
            intersectedItems.removeFirst();
		}
        if (intersectedItems.count() && intersectedItems.first() != tmpPort && intersectedItems.first() != existingPort) {
			//dw thats how a cast should look like
            NodePort *node = qgraphicsitem_cast<NodePort *>(intersectedItems.first());
			//ugly direction handling, only case where a switch is possible
            if (existingPort->portType() == NodePort::InOut) {
				//switch if non matching
                if ((node->portType() == NodePort::Out && tmpConnection->startPort() == existingPort)
                    || (node->portType() == NodePort::In && tmpConnection->endPort() == existingPort)) {
					//tmpConnector->setConnectorType(node->connectorType());
                    NodePort* old1 = tmpConnection->startPort();
                    NodePort* old2 = tmpConnection->endPort();
					//dw needed? removeItem(tmpArrow);
					//dw667 backmerge: to slow if connector is child of item
					//removeItem(tmpArrow);
                    delete tmpConnection;
                    tmpConnection = new NodeConnection(old2, old1, NULL, this);
					//dw667 backmerge: to slow if connector is child of item
					//addItem(tmpArrow);
				}
			}
			//check if uncompatible
            else if (node->portType() != NodePort::InOut && node->portType() == existingPort->portType()) {
				return;
			}

			node->setHighlight(true);
            //node->updatePosition();
            //node->update();
			//dw667 backmerge:
			node->updatePosition();
			node->update();
			
            tmpPort->setPortAlignment(node->portAlignment());
            tmpPort->setPortType(node->portType());
			//FIXME: make configurable
            tmpConnection->setBidirectional(true);

            tmpPort->updatePosition();
            tmpPort->update();
			lastHighlighted = node;
		}
		else if (lastHighlighted != NULL) {
			lastHighlighted->setHighlight(false);
            //lastHighlighted->updatePosition();
            //lastHighlighted->update();
			//dw667 backmerge:
			lastHighlighted->updatePosition();
			lastHighlighted->update();
			
			lastHighlighted = NULL;
            tmpPort->updatePosition();
            tmpPort->update();
		}

        //QLineF newLine(line->line().p1(), mouseEvent->scenePos());
		//line->setLine(newLine);
		//tmpConnector = new NodeConnector(NULL, scene, NULL, NodeConnector::InOut);
		//tmpConnector-> = new NodeConnector(NULL, scene, NULL, NodeConnector::InOut);
        tmpPort->setPos(mouseEvent->scenePos());
        tmpConnection->updatePosition();
        tmpConnection->update();
		//dw667 backmerge: was commented
        tmpPort->updatePosition();
		
        tmpPort->update();
		return;
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}


void NodeEditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << __FUNCTION__;

    if (tmpConnection != 0) {
        NodePort* startC = tmpConnection->startPort();
        NodePort* endC = tmpConnection->endPort();
		QPointF startPos(startC->mapToScene(0, 0));
		QPointF endPos(endC->mapToScene(0, 0));
		//QPointF endPos(mouseEvent->scenePos());

        tmpConnection->startPort()->setHighlight(false);
        tmpConnection->endPort()->setHighlight(false);
        //tmpArrow->startConnector()->updatePosition();
        //tmpArrow->startConnector()->update();
        //tmpArrow->endConnector()->updatePosition();
        //tmpArrow->endConnector()->update();

		QList<QGraphicsItem *> startConnectors = items(startPos);
        while(startConnectors.count() && (startConnectors.first() == tmpConnection || startConnectors.first() == tmpPort || startConnectors.first()->type() != NodePort::Type)) {
            startConnectors.removeFirst();
		}
		QList<QGraphicsItem *> endConnectors = items(endPos);
        while(endConnectors.count() && (endConnectors.first() == tmpConnection || endConnectors.first() == tmpPort || endConnectors.first()->type() != NodePort::Type)) {
            endConnectors.removeFirst();
		}

		//removeItem(tmpArrow);

        delete tmpConnection;
        tmpConnection = 0;
		//dw now done in dtor, good idea?
		//dw new
		//removeItem(tmpConnector);
        delete tmpPort;
		//tmpConnector->deleteLater();
        tmpPort = 0;
        
		if (startConnectors.count() > 0 && endConnectors.count() > 0 &&
            startConnectors.first()->type() == NodePort::Type &&
            endConnectors.first()->type() == NodePort::Type &&
            startConnectors.first() != endConnectors.first())
		{
            NodePort *startConnector =
                qgraphicsitem_cast<NodePort *>(startConnectors.first());
            NodePort *endConnector =
                qgraphicsitem_cast<NodePort *>(endConnectors.first());

			//dw new: verify again:
            if (!((startConnector->portType() == NodePort::In && endConnector->portType() == NodePort::In) || (startConnector->portType() == NodePort::Out && endConnector->portType() == NodePort::Out)))
			{
				NodeConnection *arrow = new NodeConnection(startConnector, endConnector, NULL, this);
                arrow->setColor(QColor(100,100,100)); // settings.connectionLineColor
				startConnector->addConnection(arrow);
				endConnector->addConnection(arrow);
				arrow->setZValue(-1000.0);

                arrow->startPort()->setHighlight(false);
                arrow->endPort()->setHighlight(false);
                arrow->startPort()->update();
                arrow->endPort()->update();
			}

//            arrow->setZValue(2.0);
            //addItem(arrow);
			startConnector->setHighlight(false);
			endConnector->setHighlight(false);
            //arrow->updatePosition();
			//dw new

            //startConnector->updatePosition();
            //startConnector->update();
            //endConnector->updatePosition();
            //endConnector->update();
        }
			//NOTE: assumtion that lastHighlighted is never deleted while drawing lines, otherwise pointer can go to nirvana (TODO: can we use this assumtion?)
		if (lastHighlighted != NULL) {
			lastHighlighted->setHighlight(false);
            //lastHighlighted->updatePosition();
            //lastHighlighted->update();
			lastHighlighted = NULL;
		}
		return;
    }

    //line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}


*/
