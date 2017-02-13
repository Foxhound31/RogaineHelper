#pragma once

#include <QtWidgets>

#include "NodeItem.h"
#include "NodePort.h"
#include "NodeConnection.h"

class CustomRect;
class CustomLine;


// nodes for distance measurement
class CustomRect : public QGraphicsRectItem
{
public:

    enum { Type = 107 };
    int type() const Q_DECL_OVERRIDE {return Type;}

    CustomRect (const QRectF& rect) : QGraphicsRectItem(rect) {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsFocusable);
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        _nextLine = nullptr;
        _prevLine = nullptr;
    }

    int getTotalLen(); // get total length of all connected lines
    void setNextLine(CustomLine *newLine) { _nextLine = newLine; }
    void setPrevLine(CustomLine *newLine) { _prevLine = newLine; }
    CustomLine * nextLine() { return _nextLine; }
    CustomLine * prevLine() { return _prevLine; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    CustomLine *_nextLine;
    CustomLine *_prevLine;
};




// edges for distance measurement
class CustomLine : public QGraphicsLineItem
{
public:

    enum { Type = 106 };
    int type() const Q_DECL_OVERRIDE {return Type;}

    CustomLine (const QLineF& line) : QGraphicsLineItem(line) {
        //setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemIsFocusable);
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        _nextRect = nullptr;
        _prevRect = nullptr;
    }


    int getTotalLen(); // get total length of all connected lines
    void setNextRect(CustomRect *newRect) { _nextRect = newRect; }
    void setPrevRect(CustomRect *newRect) { _prevRect = newRect; }
    CustomRect * nextRect() { return _nextRect; }
    CustomRect * prevRect() { return _prevRect; }

private:
    CustomRect * _nextRect;
    CustomRect * _prevRect;
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
    CustomLine * currentLine;

private:
    QList <QPointF> m_points;
};









