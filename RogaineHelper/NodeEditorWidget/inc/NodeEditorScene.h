#pragma once

#include <QtWidgets>

#include "NodeItem.h"
#include "NodePort.h"
#include "NodeConnection.h"


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
    bool isLineDrawing;
    QGraphicsItem * lastItem;
    QGraphicsLineItem * currentLine;

private:
    QList <QPointF> m_points;
};




