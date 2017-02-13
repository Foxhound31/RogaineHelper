#pragma once

#include <QtWidgets>
class NodeEditorWidget;



//--------------------------------------------------------------------------------
// Settings for whole Widget
//--------------------------------------------------------------------------------
struct NodeEditorSettings
{
    // Widget representation
    bool debugDraw;

    // Node representation  
    bool showNodeTitle;
    bool showNodeSmallTitle;
    bool showNodeIcon;
    bool showNodeWidget;
    bool showNodePortNames;

    // Port representation

    // Connection representation
    QColor connectionLineColor;
};





//--------------------------------------------------------------------------------
// Node Editor View - Wrapper for QGraphicsView
// Can process some events (wheelEvent, mouseEvents)
//--------------------------------------------------------------------------------
class NodeEditorView : public QGraphicsView
{
    Q_OBJECT

public:
    NodeEditorView ( NodeEditorWidget * v) : QGraphicsView(), ownerNodeEditorWidget(v) {}

protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *);
#endif
    void mousePressEvent(QMouseEvent *event);


private:
    NodeEditorSettings settings;
    NodeEditorWidget *ownerNodeEditorWidget; // "parent" widget that owns this View

};





//--------------------------------------------------------------------------------
// Widget for node editor, to be added on the Window
// Includes QGraphicsView and several control buttons (zoom, reset)
//--------------------------------------------------------------------------------
class NodeEditorWidget : public QFrame
{
    Q_OBJECT
public:
    explicit NodeEditorWidget(QWidget *parent = 0);
    void setScene(QGraphicsScene * scene);


public slots:
    void zoomIn(int level = 1)  { zoomSlider->setValue(zoomSlider->value() + level); }
    void zoomOut(int level = 1) { zoomSlider->setValue(zoomSlider->value() - level); }

private slots:
    void zoomReset();
    void zoomFit();
    void setupZoom();

private:
    NodeEditorView * nodeEditorView;
    QSlider *zoomSlider;
    QToolButton *zoomInButton;
    QToolButton *zoomOutButton;
    QToolButton *zoomResetButton;
    QToolButton *zoomFitButton;


    //QPointF target_scene_pos, target_viewport_pos;
};

















