#pragma once

#include <QtWidgets>

//#include "../src/NodeItem.h"
//#include "../src/NodePort.h"
//#include "../src/NodeConnection.h"

//class NodeItem;
//class NodeConnection;
//class NodePort;


//class ExampleBaseNode;
//class ExampleNode1;
//class NodeEditorScene;



class ExampleBaseNode : public NodeItem {
public:
    QGridLayout  *innerGridLayout;
    QHBoxLayout *topLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QHBoxLayout *bottomLayout;


    void ExampleBaseNode::setWidget(QWidget *widget) {
        //does this work for all possible wiget types
        NodeItem::setWidget(widget);
        widget->setObjectName("ExampleBaseNode");
    }


    ExampleBaseNode(QGraphicsScene *scene,
                    QMenu *contextMenu = 0,
                    QGraphicsItem *parent = 0,
                    Qt::WindowFlags wFlags = 0) :
        NodeItem(contextMenu, parent, scene, wFlags)
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


        //dw newwww
        innerGridLayout->setMargin(0);
        /*
        innerGridLayout->addItem(new QSpacerItem(0,0),0,0);
        innerGridLayout->addItem(new QSpacerItem(0,0),0,1);
        innerGridLayout->addItem(new QSpacerItem(0,0),1,0);
        innerGridLayout->addItem(new QSpacerItem(0,0),1,1);
        */


        /*
        addConnectorAndLabel("b1", NodeConnector::InOut, NodeConnector::Bottom);
        addConnectorAndLabel("righttest1", NodeConnector::InOut, NodeConnector::Right);
        addConnectorAndLabel("righttest2", NodeConnector::InOut, NodeConnector::Right);
        addConnectorAndLabel("b2", NodeConnector::InOut, NodeConnector::Bottom);
        addConnectorAndLabel("righttest3", NodeConnector::InOut, NodeConnector::Right);
        addConnectorAndLabel("lefttest1", NodeConnector::InOut, NodeConnector::Left);
        addConnectorAndLabel("lefttest2", NodeConnector::InOut, NodeConnector::Left);
        addConnectorAndLabel("lefttest3", NodeConnector::InOut, NodeConnector::Left);
        addConnectorAndLabel("b3", NodeConnector::InOut, NodeConnector::Bottom);
        addConnectorAndLabel("t1", NodeConnector::InOut, NodeConnector::Top);
        addConnectorAndLabel("t2", NodeConnector::InOut, NodeConnector::Top);
        addConnectorAndLabel("lefttest4", NodeConnector::InOut, NodeConnector::Left);
        addConnectorAndLabel("b4", NodeConnector::InOut, NodeConnector::Bottom);
        */


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



    virtual void addConnectorAndLabel(QString name,
                                      NodePort::PortType t,
                                      NodePort::PortAlignment align) {
        //int cols = innerGridLayout->columnCount();
        //int rows = innerGridLayout->rowCount();

        QLabel* l = new QLabel(name);
        if (align == NodePort::Left) {
            l->setObjectName("leftLabel");
            l->setAlignment(Qt::AlignLeft);
            if (leftLayout->count() > 1) leftLayout->addStretch();
            leftLayout->addWidget(l);
            //leftLayout->addStretch();
        }
        else if (align == NodePort::Right) {
            l->setObjectName("rightLabel");
            l->setAlignment(Qt::AlignRight);
            if (leftLayout->count() > 1) rightLayout->addStretch();
            rightLayout->addWidget(l);
            //rightLayout->addStretch();
        }
        else if (align == NodePort::Top) {
            l->setObjectName("topLabel");
            l->setAlignment(Qt::AlignCenter);
            if (leftLayout->count() > 1) topLayout->addStretch();
            topLayout->addWidget(l);
            //topLayout->addStretch();
        } else if (align == NodePort::Bottom) {
            l->setObjectName("bottomLabel");
            l->setAlignment(Qt::AlignCenter);
            if (leftLayout->count() > 1) bottomLayout->addStretch();
            bottomLayout->addWidget(l);
            //bottomLayout->addStretch();
        }

        //innerGridLayout->upd

        addConnector(new NodePort(this, scene(), l, t, align, false));
    }


//    virtual void serialize(QTextStream& out) {
//        //QString res;
//        //node
//        out << "typeId=" << QString::number(getId()) << "\n";
//        out << "uid=" << QString::number((size_t) this) << ", x=" << QString::number(pos().x()) << ", y=" << QString::number(pos().y()) << "\n";
//        //connections that start on this node
//        foreach(NodeConnector* con, connectors) {
//            foreach(NodeConnection* c, con->arrows) {
//                NodeConnector* oc = NULL;
//                if (c->startConnector() == con) {
//                    oc = c->endConnector();
//                }
//                else {
//                    oc = c->startConnector();
//                }
//                ExampleBaseNode* oi = static_cast<ExampleBaseNode*>(oc->parentItem());
//                //otherwise self-connections on this node (but not when on same connector) would be written twice, so do nothing in one of the two cases
//                if (this == oi && this->connectors.indexOf(con) < oi->connectors.indexOf(oc)) {
//                    continue;
//                }
//                if (c->startConnector() == con) {
//                    out << "startNodeConnector=" << QString::number(this->connectors.indexOf(con));
//                    out << ", endNodeUid=" << QString::number((size_t) oi);
//                    out << ", endNodeConnector=" << QString::number(oi->connectors.indexOf(oc));
//                }
//                else {
//                    out << "endNodeConnector=" << QString::number(this->connectors.indexOf(con));
//                    out << ", startNodeUid=" << QString::number((size_t) oi);
//                    out << ", startNodeConnector=" << QString::number(oi->connectors.indexOf(oc));
//                }
//                out << "\n";
//            }
//        }
//        out << "\n";
//    }

//    virtual void deserialize(QTextStream& out, QMap<int, ExampleBaseNode*>& map) {
//        //typeId is read by caller
//        QString  l;
//        if ((l=out.readLine()) != "") {
//            QStringList list = l.split(",");
//            int id = list[0].split("=")[1].toInt();
//            int x = list[1].split("=")[1].toInt();
//            int y = list[2].split("=")[1].toInt();
//            this->setPos(x, y);
//            map[id] = this;
//        }
//        while ((l=out.readLine()) != "") {
//            bool isStart = l.startsWith("startNodeConnector=");
//            QStringList list = l.split(",");
//            int c = list[0].split("=")[1].toInt();
//            int oid = list[1].split("=")[1].toInt();
//            int oc = list[2].split("=")[1].toInt();
//            //only the second node creates the connection
//            if (map.contains(oid)) {
//                NodeItem* other = map[oid];
//                NodeConnector* thisCon = this->connectors[c];
//                NodeConnector* oCon = other->connectors[oc];
//                NodeConnection* newCon = NULL;
//                if (isStart) {
//                    newCon = new NodeConnection(thisCon, oCon, NULL, this->scene());
//                }
//                else {
//                    newCon = new NodeConnection(oCon, thisCon, NULL, this->scene());
//                }
//                //FIXME: should not be job of user
//                thisCon->addConnection(newCon);
//                oCon->addConnection(newCon);
//                newCon->setZValue(-1000.0);
//            }
//        }
//    }

    virtual int getId() = 0;
};



//"ExampleNode1: dialog, single in, only one connection"
class ExampleNode1 : public ExampleBaseNode
{
public:
    ExampleNode1(QGraphicsScene *scene,
                 QMenu *contextMenu = nullptr,
                 QGraphicsItem *parent = nullptr);
    int getId() { return 1; }
};


