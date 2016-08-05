#pragma once

#include <QtWidgets>

#include "NodeItem.h"
#include "NodePort.h"
#include "NodeConnection.h"




// edges for distance measurement
class CustomLine : public QGraphicsLineItem
{
    CustomLine (const QLineF& line) : QGraphicsLineItem(line) {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsFocusable);
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        nextRect = nullptr;
        prevRect = nullptr;
    }

    // get total length of all connected lines
    int getTotalLen() {
        int result = 0;
        return result;
    }

private:
    QGraphicsRectItem * nextRect;
    QGraphicsRectItem * prevRect;
};


// nodes for distance measurement
class CustomRect : public QGraphicsRectItem
{
public:
    CustomRect (const QRectF& rect) : QGraphicsRectItem(rect) {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsFocusable);
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        nextLine = nullptr;
        prevLine = nullptr;
    }

    void setNextLine(QGraphicsLineItem *newLine) { nextLine = newLine; }
    void setPrevLine(QGraphicsLineItem *newLine) { prevLine = newLine; }

    // get total length of all connected lines
    int getTotalLen() {
        int result = 0;
//        CustomRect * tmpItem;
//        if (nextLinePtr && nextItem) {
//            tmpItem = this;
//            while(tmpItem) {
//                result += nextLinePtr->line().length();
//                tmpItem = tmpItem->nextItem;
//            }
//        }
//        if (prevLinePtr) {
//        }
        return result;
    }


    QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        if (change == ItemPositionChange && scene()) {
            // value is the new position.
            QPointF newPos = value.toPointF();
            updateLnes(newPos);
        }
        return QGraphicsItem::itemChange(change, value);
    }

    void updateLnes(QPointF newPos) {
        QPointF newCenterPos = QPointF(newPos.x() + rect().center().x(), newPos.y() + rect().center().y());
        // Move the required point of the line to the center of the elipse
        if (nextLine) {
            nextLine->setLine(QLineF(newCenterPos, nextLine->line().p2()));
        }
        if (prevLine) {
            prevLine->setLine(QLineF(prevLine->line().p1(), newCenterPos));
        }
    }

private:
    QGraphicsLineItem *nextLine;
    QGraphicsLineItem *prevLine;
};


















typedef enum sceneClickModeEnum {
    CLICKMODE_SELECT,
    CLICKMODE_ADD_RECT,
    CLICKMODE_ADD_CIRCLE,
} sceneClickMode;



class  NodeEditorScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit NodeEditorScene(QWidget *parent = nullptr);

    void deleteSelectedItems();
    void setClickMode(sceneClickMode mode) {clickMode = mode;}


signals:
    void nodeItemInserted(NodeItem *item);
    void itemInserted(QGraphicsItem *item);
    void itemSelected(QGraphicsItem *item);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

    //void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    //void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    bool leftButtonDown;
    QPointF startPoint;
    NodeConnection* tmpConnection;
    NodePort* tmpPort;
    NodePort* existingPort;
    NodePort* lastHighlighted;
    NodeConnection *conn;


    sceneClickMode clickMode;

    // line chain
    bool isLineDrawing;
    CustomRect * lastItem;
    QGraphicsLineItem * currentLine;

private:
    QList <QPointF> m_points;
};









