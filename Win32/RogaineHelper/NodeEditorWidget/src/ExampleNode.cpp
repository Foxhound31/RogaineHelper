#include <QtWidgets>

#include "inc/NodePort.h"
#include "inc/NodeItem.h"


class ExampleNode : public NodeItem
{
public:
    QGridLayout *innerGridLayout;
    QHBoxLayout *topLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QHBoxLayout *bottomLayout;


    ExampleNode(QGraphicsScene *scene,
                QMenu *contextMenu = 0,
                QGraphicsItem *parent = 0,
                Qt::WindowFlags wFlags = 0) :
        NodeItem(NULL, wFlags, parent)
    {

        //dw new
        QFrame *outterFrame = new QFrame;
        //QWidget *outterFrame = new QWidget;
        outterFrame->setObjectName("outterFrame");
        //QFormLayout *formlayout = new QFormLayout;

        QVBoxLayout* outterLayout = new QVBoxLayout;
        outterLayout->setMargin(0);


        //QGroupBox *innerFrame = new QGroupBox(outterFrame/*"Contact Details"*/);
        //QFrame *innerFrame = new QFrame(outterFrame/*"Contact Details"*/);
        //QFrame *innerFrame = new QFrame(outterFrame);
        //QWidget *innerFrame = new QWidget(outterFrame);
        QFrame *innerFrame = new QFrame();
        innerFrame->setObjectName("innerFrame");

        //QLineEdit *numberEdit = new QLineEdit;
        //QFormLayout *layout = new QFormLayout;
        //QGridLayout  *innerGridLayout = new QGridLayout;
        innerGridLayout = new QGridLayout;
        topLayout = new QHBoxLayout();
        //innerGridLayout->addLayout(topLayout, 0, 0, 1, 2/*, Qt::AlignCenter*/);
        QFrame *topFrame = new QFrame;
        topFrame->setLayout(topLayout);
        innerGridLayout->addWidget(topFrame, 0, 0, 1, 2/*, Qt::AlignCenter*/);
        leftLayout = new QVBoxLayout();
        //innerGridLayout->addLayout(leftLayout, 1, 0, 1, 1/*, Qt::AlignLeft*/);
        QFrame *leftFrame = new QFrame;
        leftFrame->setLayout(leftLayout);
        innerGridLayout->addWidget(leftFrame, 1, 0, 1, 1/*, Qt::AlignLeft*/);
        rightLayout = new QVBoxLayout();
        //innerGridLayout->addLayout(rightLayout, 1, 1, 1, 1/*, Qt::AlignRight*/);
        QFrame *rightFrame = new QFrame;
        rightFrame->setLayout(rightLayout);
        innerGridLayout->addWidget(rightFrame, 1, 1, 1, 1/*, Qt::AlignRight*/);
        bottomLayout = new QHBoxLayout();
        //innerGridLayout->addLayout(bottomLayout, 2, 0, 1, 2/*, Qt::AlignCenter*/);
        QFrame *bottomFrame = new QFrame;
        bottomFrame->setLayout(bottomLayout);
        innerGridLayout->addWidget(bottomFrame, 2, 0, 1, 2/*, Qt::AlignCenter*/);

        topLayout->setMargin(0);
        leftLayout->setMargin(0);
        rightLayout->setMargin(0);
        bottomLayout->setMargin(0);


        innerGridLayout->setMargin(0);

        //formlayout->addWidget(innerFrame);
        //setWidget(innerFrame);
        //outterLayout->addWidget(innerFrame);

        //outterLayout->addLayout(innerGridLayout);
        innerFrame->setLayout(innerGridLayout);
        outterLayout->addWidget(innerFrame);

        //innerFrame->setLayout(innerGridLayout);
        outterFrame->setLayout(outterLayout);
        setWidget(outterFrame);

        outterFrame->setObjectName("ExampleBaseNode");
    }

};


