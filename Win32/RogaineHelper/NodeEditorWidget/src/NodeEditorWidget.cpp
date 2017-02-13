#include <QtGui>
#include <QtWidgets>
#include <QGraphicsSceneEvent>
#include <QDebug>
#include <QGraphicsItem>

#include "../inc/NodeEditorWidget.h"
#include "NodeItem.h"
#include "NodeConnection.h"
#include "NodePort.h"


//--------------------------------------------------------------------------------
// Node Editor View
//--------------------------------------------------------------------------------
#ifndef QT_NO_WHEELEVENT
void NodeEditorView::wheelEvent(QWheelEvent *e)
{
    // To scroll only if Ctrl is pressed
    //if (e->modifiers() & Qt::ControlModifier) {
    //
    //} else {
    //    QGraphicsView::wheelEvent(e);
    // }


    if (e->delta() > 0) {
        ownerNodeEditorWidget->zoomIn(6);
    }
    else {
        ownerNodeEditorWidget->zoomOut(6);
    }
    e->accept();

}
#endif


//--------------------------------------------------------------------------------
void NodeEditorView::mousePressEvent(QMouseEvent *event)
{
   qDebug() << Q_FUNC_INFO << event->screenPos();
   QGraphicsView::mousePressEvent(event);
}









//--------------------------------------------------------------------------------
// Widget for node editor
//--------------------------------------------------------------------------------

void NodeEditorWidget::setScene(QGraphicsScene * scene)
{
    nodeEditorView->setScene(scene);
}

NodeEditorWidget::NodeEditorWidget(QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(Sunken | StyledPanel);

    // Create and configure QGraphicsView, set Scene for it
    nodeEditorView = new NodeEditorView(this);
    nodeEditorView->setRenderHint(QPainter::Antialiasing, false);
    nodeEditorView->setDragMode(QGraphicsView::RubberBandDrag);
    nodeEditorView->setMouseTracking(true);
    nodeEditorView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    nodeEditorView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    nodeEditorView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    nodeEditorView->setInteractive(true);
    //nodeEditorView->setBackgroundBrush(QPixmap(":/No-Ones-Laughing-3.jpg"));


    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QSize iconSize(size, size);


    zoomInButton = new QToolButton;
    zoomInButton->setText(tr("+"));
    zoomInButton->setAutoRepeat(true);
    zoomInButton->setAutoRepeatInterval(33);
    zoomInButton->setAutoRepeatDelay(0);
    //zoomInButton->setIcon(QPixmap(":/zoomin.png"));
    zoomInButton->setIconSize(iconSize);
    zoomInButton->setToolTip("Zoom In");

    zoomOutButton = new QToolButton;
    zoomOutButton->setText(tr("-"));
    zoomOutButton->setAutoRepeat(true);
    zoomOutButton->setAutoRepeatInterval(33);
    zoomOutButton->setAutoRepeatDelay(0);
    //zoomOutButton->setIcon(QPixmap(":/zoomout.png"));
    zoomOutButton->setIconSize(iconSize);
    zoomOutButton->setToolTip("Zoom Out");

    zoomResetButton = new QToolButton;
    zoomResetButton->setText(tr("100%"));
    zoomResetButton->setEnabled(true);
    //zoomResetButton->setIcon(QPixmap(":/zoomreset.png"));
    zoomResetButton->setToolTip("Reset Zoom to 100%");

    zoomFitButton = new QToolButton;
    zoomFitButton->setText(tr("Fit"));
    zoomFitButton->setEnabled(true);
    //zoomFitButton->setIcon(QPixmap(":/zoomfit.png"));
    zoomFitButton->setToolTip("Fit into area");

    zoomSlider = new QSlider;
    zoomSlider->setFixedHeight(200);
    zoomSlider->setMinimum(1);
    zoomSlider->setMaximum(200);
    zoomSlider->setValue(100);
    zoomSlider->setTickPosition(QSlider::NoTicks);
    zoomSlider->setToolTip("Scroll to zoom");


    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout;
    zoomSliderLayout->addWidget(zoomInButton);
    zoomSliderLayout->addWidget(zoomSlider);
    zoomSliderLayout->addWidget(zoomOutButton);
    zoomSliderLayout->addWidget(zoomResetButton);
    zoomSliderLayout->addWidget(zoomFitButton);
    zoomSliderLayout->setSpacing(0);
    zoomSliderLayout->setMargin(0);


    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(nodeEditorView, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    topLayout->setSpacing(0);
    topLayout->setMargin(1);
    setLayout(topLayout);

    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupZoom()));
    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));
    connect(zoomResetButton, SIGNAL(clicked()), this, SLOT(zoomReset()));
    connect(zoomFitButton, SIGNAL(clicked()), this, SLOT(zoomFit()));

    setupZoom();
}

//--------------------------------------------------------------------------------
void NodeEditorWidget::zoomReset()
{
    zoomSlider->setValue(100);
    setupZoom();
    QGraphicsScene * scene = nodeEditorView->scene();
}

//--------------------------------------------------------------------------------
void NodeEditorWidget::zoomFit()
{
    nodeEditorView->fitInView(nodeEditorView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

//--------------------------------------------------------------------------------
void NodeEditorWidget::setupZoom()
{
    qreal scale = qPow(qreal(2), (zoomSlider->value() - 100) / qreal(50));
    QMatrix matrix;
    matrix.scale(scale, scale);
    nodeEditorView->setMatrix(matrix); 

//    nodeEditorView->scale(scale, scale);
//    nodeEditorView->centerOn(target_scene_pos);
//    QPointF delta_viewport_pos = target_viewport_pos - QPointF(nodeEditorView->viewport()->width() / 2.0,
//                                                               nodeEditorView->viewport()->height() / 2.0);
//    QPointF viewport_center = nodeEditorView->mapFromScene(target_scene_pos) - delta_viewport_pos;
//    nodeEditorView->centerOn(nodeEditorView->mapToScene(viewport_center.toPoint()));
}






