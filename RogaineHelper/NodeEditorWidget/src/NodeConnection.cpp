#include <QtGui>
#include <QtWidgets>
#include <math.h>

#include "NodeItem.h"
#include "NodeConnection.h"
#include "NodePort.h"


const qreal Pi = 3.14;

NodeConnection::~NodeConnection(){
    if (this->mStartPort != NULL) {
        this->mStartPort->removeConnection(this);
        this->mStartPort = NULL;
	}
    if (this->mEndPort != NULL) {
        this->mEndPort->removeConnection(this);
        this->mEndPort = NULL;
	}

	if (scene() != NULL) {
		this->scene()->removeItem(this);
	}
}

//! [0]
NodeConnection::NodeConnection(NodePort *startPort,
                               NodePort *endPort,
                               QGraphicsItem *parent,
                               QGraphicsScene *scene,
                               bool bidirectional)
         : QGraphicsPathItem(parent)
{
	//setCacheMode(DeviceCoordinateCache);

    mStartPort = startPort;
    mEndPort = endPort;
    setFlag(QGraphicsItem::ItemIsSelectable, true);

	setBidirectional(bidirectional);

	//line
    mColor = Qt::black;
	//path
	//mColor = Qt::black;
    setPen(QPen(mColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	arrowSize=10;

	setZValue(-1000.0);

	 //dw new4 already done here, but why used in other places?
	 /*/if (scene != NULL){
		scene->addItem(this);
	 }*/

	//dw new4: is this a problem? check it
    //setCacheMode(DeviceCoordinateCache);

	QPointF controlPoint1;
	QPointF controlPoint2;
	recreatePath(controlPoint1, controlPoint2);
}
//! [0]

//! [1]
QRectF NodeConnection::boundingRect() const
{
    
	/*//line
	qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);*/
	

	qreal extra = 3;
	return QGraphicsPathItem::boundingRect().normalized().adjusted(-extra, -extra, extra, extra);
	
}
//! [1]

//! [2]
QPainterPath NodeConnection::shape() const
{
	/*
	//line
    QPainterPath path = QGraphicsLineItem::shape();
    //path.addPolygon(arrowHead);
    return path;
	*/

	//return QGraphicsPathItem::shape();
	QPainterPath path = QGraphicsPathItem::shape();
    //path.addPolygon(arrowHead);
    return path;
}

void NodeConnection::updatePosition() {
	prepareGeometryChange();
	update();
}

////! [3]
////dw667: DO ONLY CALL FROM PAINT METHOD!!!
//void NodeConnection::recreatePath(QPointF& controlPoint1, QPointF& controlPoint2)
//{
//	//line
//	//QLineF line(mapFromItem(mStartConnector, 0, 0), mapFromItem(mEndConnector, 0, 0));
//    //setLine(line);
//
//	//cubic spline?
//	//QPointF a(mapFromItem(mStartConnector, 0, 0) - QPointF(mStartConnector->mRadius,0));
//	//QPointF b(mapFromItem(mEndConnector, 0, 0) - QPointF(mStartConnector->mRadius,0));
//	QPointF a(mapFromItem(mStartConnector, 0, 0));
//	QPointF b(mapFromItem(mEndConnector, 0, 0));
//	int dir1 = 1;
//	//we just think about the case where the connector is left, so just multiply dir where needed
//	if (mStartConnector->connectorAlignment() == NodeConnector::Right) { // right) {
//		dir1 = -1;
//		//a.setX(mapFromItem(mStartConnector, 0, 0).x() + mStartConnector->mRadius);
//	}
//	int dir2 = 1;
//	if (mEndConnector->connectorAlignment() == NodeConnector::Right) { // right) {
//		dir2 = -1;
//		//b.setX(mapFromItem(mEndConnector, 0, 0).x() + mStartConnector->mRadius);
//	}
//
//	//define spline "mRadius"
//	qreal spline_mRadius = 100.0;
//	//dw new version
//	QPointF l;
//	QPointF r;
//	NodeConnector *ri;
//	NodeConnector *li;
//	bool aIsLeft = true;
//	qreal dist = QLineF(a, b).length();
//	qreal diffx = abs(a.x() - b.x());
//	qreal diffy = abs(a.y() - b.y());
//	//QPointF newL;
//	//QPointF newR;
//	if (a.x() < b.x()) {
//		l = a;
//		r = b;
//		li = mStartConnector;
//		ri = mEndConnector;
//	}
//	else {
//		l = b;
//		r = a;
//		li = mEndConnector;
//		ri = mStartConnector;
//		aIsLeft = false;
//	}
//	/*
//	if (r.x() - l.x() > 5 * abs(r.y()-l.y()) && !li->right && ri->right) {
//		newL = l - QPointF(dist/2.0, dist/2.0);
//		newR = r + QPointF(dist/2.0, -dist/2.0);
//	}
//	else {
//		newL = l + (li->right ? 1.0 : -1.0) * QPointF(dist/2.0, 0);
//		newR = r + (ri->right ? 1.0 : -1.0) * QPointF(dist/2.0, 0);
//	}
//	*/
//	
//
//	//QPointF controlPoint1;
//	//QPointF controlPoint2;
//	if (r.x() - l.x() > 5 * diffy && li->connectorAlignment() == NodeConnector::Left && ri->connectorAlignment() == NodeConnector::Right) {
//		controlPoint1 = l - QPointF(dist/4.0 + spline_mRadius , dist/4.0 + spline_mRadius);
//		controlPoint2 = r + QPointF(dist/4.0 + spline_mRadius, -dist/4.0 + spline_mRadius);
//	}
//	else {
//		controlPoint1 = l + (li->connectorAlignment() == NodeConnector::Right ? 1.0 : -1.0) * QPointF(dist/2 + (diffy > spline_mRadius ? spline_mRadius : 0), 0);
//		controlPoint2 = r + (ri->connectorAlignment() == NodeConnector::Right ? 1.0 : -1.0) * QPointF(dist/2 + (diffy > spline_mRadius ? spline_mRadius : 0), 0);
//	}
//	
//
//	//dw new5
//	//dw667: was active
//	QPainterPath p(a);
//	//QPainterPath p = path();
//	if (aIsLeft) {
//		p.cubicTo(controlPoint1, controlPoint2, b);
//	}
//	else {
//		p.cubicTo(controlPoint2, controlPoint1, b);
//	}
//
//
//	/*
//	//if directions are different and wrong direction, use y also
//	qreal diffy1 = 0;
//	qreal diffy2 = 0;
//	//y goes from top to bottom!!!!
//	if (dir1*dir2<0) {
//		if (a.y() < b.y()) {
//			diffy1 = spline_mRadius;
//			diffy2 = -spline_mRadius;
//		}
//		else {
//			diffy1 = -spline_mRadius;
//			diffy2 = spline_mRadius;
//		}
//	}
//	QPointF h1(a.x()-spline_mRadius*dir1, a.y() + diffy1);
//	QPointF h2(b.x()-spline_mRadius*dir2, b.y() + diffy1);
//	p.cubicTo(h1, h2, b);
//	*/
//
//
//	//////////////////////
//	//arrow head
//
//	/*
//	QPointF headStartP = p.pointAtPercent(p.percentAtLength(p.length() - mEndConnector->mRadius - arrowSize));
//	QPointF headEndP = p.pointAtPercent(p.percentAtLength(p.length() - mEndConnector->mRadius));
//	QLineF arrowMiddleLine(headStartP, headEndP);
//	//QLineF normHead = arrowMiddleLine.normalVector();
//	arrowMiddleLine.unitVector();
//	QPointF normHead(arrowMiddleLine.dy(), -arrowMiddleLine.dx());
//	QPointF arrowP1 = headStartP + normHead * 0.4;
//	QPointF arrowP2 = headStartP - normHead * 0.4;
//
//	arrowHead.clear();
//	//dw668
//	arrowHead << headEndP << arrowP1 << arrowP2 /*<< headEndP*//*;
//	p.addPolygon(arrowHead);
//	*/
//
//	//createArrows(p);
//
//	//////////////////
//	//dw667: was active
//	this->setPath(p);
//}
////! [3]


//new try:
//dw667: DO ONLY CALL FROM PAINT METHOD!!!
void NodeConnection::recreatePath(QPointF& controlPoint1, QPointF& controlPoint2)
{
    QPointF a(mapFromItem(mStartPort, 0, 0));
    QPointF b(mapFromItem(mEndPort, 0, 0));

	qreal dist = QLineF(a, b).length();
	qreal diffx = abs(a.x() - b.x());
	qreal diffy = abs(a.y() - b.y());

	QPointF left;
	QPointF right;
	NodePort* leftConn;
	NodePort* rightConn;
	if (a.x() < b.x()) {
		left = a;
        leftConn = mStartPort;
		right = b;
        rightConn = mEndPort;
	}
	else {
		left = b;
        leftConn = mEndPort;
		right = a;
        rightConn = mStartPort;
	}

	QPointF bottom;
	QPointF top;
	NodePort* bottomConn;
	NodePort* topConn;
	if (a.y() < b.y()) {
		top = a;
        topConn = mStartPort;
		bottom = b;
        bottomConn = mEndPort;
	}
	else {
		top = b;
        topConn = mEndPort;
		bottom = a;
        bottomConn = mStartPort;
	}

	
	controlPoint1 = a;
	controlPoint2 = b;
	//how much to move control point from start or end point, default as used for simple case
	qreal moveX = 0.45 * diffx;
	qreal moveY = 0.45 * diffy;

	QSizeF combinedSize(30,30);
	if (leftConn->parentItem() != NULL) {
		combinedSize.setWidth(leftConn->parentItem()->boundingRect().width());
		combinedSize.setHeight(leftConn->parentItem()->boundingRect().height());
	}
	else {
		combinedSize.setWidth(5 * leftConn->mRadius);
		combinedSize.setWidth(5 * leftConn->mRadius);
	}
	if (rightConn->parentItem() != NULL) {
		combinedSize.setWidth(combinedSize.width() + rightConn->parentItem()->boundingRect().width());
		combinedSize.setHeight(combinedSize.height() + rightConn->parentItem()->boundingRect().height());
	}
	else {
		combinedSize.setWidth(combinedSize.width() + 5 * rightConn->mRadius);
		combinedSize.setHeight(combinedSize.height() + 5 * rightConn->mRadius);
	}
	
	

    if (leftConn->portAlignment() == rightConn->portAlignment() && leftConn->portAlignment() == NodePort::Left && right.x() - left.x() < combinedSize.width()/2.0) {
		controlPoint1.setX(controlPoint1.x() - moveY/2.0 /*- combinedSize.width()/2.0*/);
		controlPoint2.setX(controlPoint2.x() - moveY/2.0 /*- combinedSize.width()/2.0*/);
	}
    else if (leftConn->portAlignment() == rightConn->portAlignment() && leftConn->portAlignment() == NodePort::Right && right.x() - left.x() < combinedSize.width()/2.0) {
		controlPoint1.setX(controlPoint1.x() + moveY/2.0 /*+ combinedSize.width()/2.0*/);
		controlPoint2.setX(controlPoint2.x() + moveY/2.0 /*+ combinedSize.width()/2.0*/);
	}
    else if (leftConn->portAlignment() == rightConn->portAlignment() && leftConn->portAlignment() == NodePort::Top && bottom.y() - top.y() < combinedSize.height()/2.0) {
		controlPoint1.setY(controlPoint1.y() - moveX/2.0 /*- combinedSize.height()/2.0*/);
		controlPoint2.setY(controlPoint2.y() - moveX/2.0 /*- combinedSize.height()/2.0*/);
	}
    else if (leftConn->portAlignment() == rightConn->portAlignment() && leftConn->portAlignment() == NodePort::Bottom && bottom.y() - top.y() < combinedSize.height()/2.0) {
		controlPoint1.setY(controlPoint1.y() + moveX/2.0 /*+ combinedSize.height()/2.0*/);
		controlPoint2.setY(controlPoint2.y() + moveX/2.0 /*+ combinedSize.height()/2.0*/);
	}
	else
	//the simple case, they face each other the "good" way
    if (leftConn->portAlignment() != NodePort::Left && rightConn->portAlignment() != NodePort::Right
            && topConn->portAlignment() != NodePort::Top && bottomConn->portAlignment() != NodePort::Bottom) {

		//very simple: straight line
		//controlPoint1 = a + 0.3 * (b-a);
		//controlPoint2 = a + 0.7 * (b-a);

		controlPoint1 = a;
		controlPoint2 = b;
		//how much to move control point from start or end point
		qreal moveX = 0.45 * diffx;
		qreal moveY = 0.45 * diffy;
        if (dist > 5 * (mStartPort->mRadius + arrowSize)) {
			/* does mess up good case because moves there too
			if (abs(diffx-diffy) > 0.3 * dist) {
				moveX += abs(diffx-diffy);
				moveY += abs(diffx-diffy);
			}
			*/
				
            if (moveX < 3 * (mStartPort->mRadius + arrowSize)) {
                moveX = 3 * (mStartPort->mRadius + arrowSize);
			}
            if (moveY < 3 * (mStartPort->mRadius + arrowSize)) {
                moveY = 3 * (mStartPort->mRadius + arrowSize);
			}
		}

        if (mStartPort->portAlignment() == NodePort::Left) {
			controlPoint1.setX(controlPoint1.x() - moveX);
		}
        else if (mStartPort->portAlignment() == NodePort::Right) {
			controlPoint1.setX(controlPoint1.x() + moveX);
		}
        else if (mStartPort->portAlignment() == NodePort::Bottom) {
			controlPoint1.setY(controlPoint1.y() + moveY);
		}
        else if (mStartPort->portAlignment() == NodePort::Top) {
			controlPoint1.setY(controlPoint1.y() - moveY);
		}

        if (mEndPort->portAlignment() == NodePort::Left) {
			controlPoint2.setX(controlPoint2.x() - moveX);
		}
        else if (mEndPort->portAlignment() == NodePort::Right) {
			controlPoint2.setX(controlPoint2.x() + moveX);
		}
        else if (mEndPort->portAlignment() == NodePort::Bottom) {
			controlPoint2.setY(controlPoint2.y() + moveY);
		}
        else if (mEndPort->portAlignment() == NodePort::Top) {
			controlPoint2.setY(controlPoint2.y() - moveY);
		}
	}
	//the bad case, method needs cleanup
	else {
		controlPoint1 = a;
		controlPoint2 = b;
		qreal maxMove = 0.5 * dist;
		moveX = 0.5 * dist;
		moveY = 0.5 * dist;
        if (mStartPort->ownerItem != NULL) {
            maxMove = 1 * (mStartPort->ownerItem->rect().width() + mStartPort->ownerItem->rect().height());
		}
        else if (mEndPort->ownerItem != NULL) {
            maxMove = 1 * (mEndPort->ownerItem->rect().width() + mEndPort->ownerItem->rect().height());
		}
		if (moveX > maxMove) {
			moveX = maxMove + 0.1 * (moveX-maxMove);
		}
		if (moveY > maxMove) {
			moveY = maxMove + 0.1 * (moveY-maxMove);
		}
        if (mStartPort->portAlignment() == NodePort::Left) {
			moveX *= -1;
            if ((mStartPort == topConn) == (mStartPort == rightConn)) {
				moveY *= -1;//relevantHeight;
			}
		}
        else if (mStartPort->portAlignment() == NodePort::Right) {
			//moveX *= 1;
            if ((mStartPort == topConn) == (mStartPort == leftConn)) {
				moveY *= -1;//relevantHeight;
			}
		}
        else if (mStartPort->portAlignment() == NodePort::Bottom) {
			//moveY *= 1;
            if ((mStartPort == leftConn) == (mStartPort == topConn)) {
				moveX *= -1;
			}
		}
        else if (mStartPort->portAlignment() == NodePort::Top) {
			moveY *= -1;
            if ((mStartPort == leftConn) == (mStartPort == bottomConn)) {
				moveX *= -1;
			}
		}

		/*
		if (mStartConnector->connectorAlignment() == mEndConnector->connectorAlignment()) {
			moveX *= 2;
			moveY *= 2;
		}
		*/

		//ugly shit: handle some cases that don't look nice
        if (mEndPort == topConn && topConn->portAlignment() == NodePort::Top && (bottomConn->portAlignment() == NodePort::Left || bottomConn->portAlignment() == NodePort::Right)) {
			moveY *= -1;
			//moveY = 0;
		}
        else if (mEndPort == bottomConn && bottomConn->portAlignment() == NodePort::Bottom && (topConn->portAlignment() == NodePort::Left || topConn->portAlignment() == NodePort::Right)) {
			moveY *= -1;
			//moveY = 0;
		}
        else if (mEndPort == leftConn && leftConn->portAlignment() == NodePort::Left && (rightConn->portAlignment() == NodePort::Top || rightConn->portAlignment() == NodePort::Bottom)) {
			moveX *= -1;
			//moveX = 0;
		}
        else if (mEndPort == rightConn && rightConn->portAlignment() == NodePort::Right && (leftConn->portAlignment() == NodePort::Top || leftConn->portAlignment() == NodePort::Bottom)) {
			moveX *= -1;
			//moveX = 0;
		}

		controlPoint1.setX(controlPoint1.x() + moveX);
		controlPoint1.setY(controlPoint1.y() + moveY);


		moveX = 0.5 * dist;
		moveY = 0.5 * dist;
		// if start was null, then it was already set to end.
        if (mStartPort->ownerItem != NULL && mEndPort->ownerItem != NULL) {
            maxMove = 1 * (mEndPort->ownerItem->rect().width() + mEndPort->ownerItem->rect().height());
		}
		if (moveX > maxMove) {
			moveX = maxMove + 0.1 * (moveX-maxMove);
		}
		if (moveY > maxMove) {
			moveY = maxMove + 0.1 * (moveY-maxMove);
		}
        if (mEndPort->portAlignment() == NodePort::Left) {
			moveX *= -1;
            if ((mEndPort == topConn) == (mEndPort == rightConn)) {
				moveY *= -1;//relevantHeight;
			}
		}
        else if (mEndPort->portAlignment() == NodePort::Right) {
			//moveX *= 1;
            if ((mEndPort == topConn) == (mEndPort == leftConn)) {
				moveY *= -1;//relevantHeight;
			}
		}
        else if (mEndPort->portAlignment() == NodePort::Bottom) {
			//moveY *= 1;
            if ((mStartPort == leftConn) == (mStartPort == topConn)) {
				moveX *= -1;
			}
		}
        else if (mEndPort->portAlignment() == NodePort::Top) {
			moveY *= -1;
            if ((mStartPort == leftConn) == (mStartPort == bottomConn)) {
				moveX *= -1;
			}
		}

		/*
		if (mStartConnector->connectorAlignment() == mEndConnector->connectorAlignment()) {
			moveX *= 2;
			moveY *= 2;
		}*/


		//ugly shit: handle some cases that don't look nice
        if (mStartPort == topConn && topConn->portAlignment() == NodePort::Top && (bottomConn->portAlignment() == NodePort::Left || bottomConn->portAlignment() == NodePort::Right)) {
			moveY *= -1;
			//moveY = 0;
		}
        else if (mStartPort == bottomConn && bottomConn->portAlignment() == NodePort::Bottom && (topConn->portAlignment() == NodePort::Left || topConn->portAlignment() == NodePort::Right)) {
			moveY *= -1;
			//moveY = 0;
		}
        else if (mStartPort == leftConn && leftConn->portAlignment() == NodePort::Left && (rightConn->portAlignment() == NodePort::Top || rightConn->portAlignment() == NodePort::Bottom)) {
			moveX *= -1;
			//moveX = 0;
		}
        else if (mStartPort == rightConn && rightConn->portAlignment() == NodePort::Right && (leftConn->portAlignment() == NodePort::Top || leftConn->portAlignment() == NodePort::Bottom)) {
			moveX *= -1;
			//moveX = 0;
		}

		controlPoint2.setX(controlPoint2.x() + moveX);
		controlPoint2.setY(controlPoint2.y() + moveY);
	}


	QPainterPath p(a);
	p.cubicTo(controlPoint1, controlPoint2, b);
	this->setPath(p);
}


//FIXME: width and length for arrow
QPolygonF NodeConnection::createArrowPoly(QPainterPath& p, NodePort* conn) {
	float arrowStartPercentage;
	float arrowEndPercentage;
	
    if (conn == mEndPort) {
		arrowStartPercentage = p.percentAtLength(p.length() - conn->mRadius - arrowSize);
		arrowEndPercentage = p.percentAtLength(p.length() - conn->mRadius);
	}
	else {
		//assuming is start connector, should throw exception otherwise?
		arrowStartPercentage = p.percentAtLength(conn->mRadius + arrowSize);
		arrowEndPercentage = p.percentAtLength(conn->mRadius);
	}
	QPointF headStartP = p.pointAtPercent(arrowStartPercentage);
	QPointF headEndP = p.pointAtPercent(arrowEndPercentage);
	QLineF arrowMiddleLine(headStartP, headEndP);
	//QLineF normHead = arrowMiddleLine.normalVector();
	arrowMiddleLine.unitVector();
	QPointF normHead(arrowMiddleLine.dy(), -arrowMiddleLine.dx());
	QPointF arrowP1 = headStartP + normHead * 0.4;
	QPointF arrowP2 = headStartP - normHead * 0.4;

	QPolygonF arrowHeadEnd;
	arrowHeadEnd << headEndP << arrowP1 << arrowP2 /*<< headEndP*/;
	return arrowHeadEnd;
}


bool NodeConnection::setBidirectional(bool bidirectional) {
    mBidirectional =  bidirectional && mStartPort->portType() == NodePort::InOut && mEndPort->portType() == NodePort::InOut;
	return mBidirectional;
}
bool NodeConnection::bidirectional() {
	return mBidirectional;
}


void NodeConnection::debugPaint(QPainter *painter, QPointF& controlPoint1, QPointF& controlPoint2) {

	//dw debug
	static int i = 0, j=0, k=0;

	QPen mPen = painter->pen();
	QColor ctrlPtCol(i=(i+19)%256 , j=(j+51)%256, k=(k+11)%256);
	mPen.setColor(ctrlPtCol);
	painter->setPen(mPen);

	//painter->drawPath(shape()); // to see item.
	painter->fillRect(boundingRect(), QColor(i=(i+19)%256 , j=(j+51)%256, k=(k+11)%256)); // to see item.

	//painter->setPen(Qt::NoPen);
    painter->setBrush(ctrlPtCol);

	//painter->drawPoint(controlPoint1);
	//painter->drawPoint(controlPoint2);
	painter->drawEllipse(controlPoint1.x()-2, controlPoint1.y()-2, 5, 5);
	painter->drawEllipse(controlPoint2.x()-2, controlPoint2.y()-2, 5, 5);
}


//! [4]
void NodeConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w)
{
	Q_UNUSED(option);
    Q_UNUSED(w);

	//dw 699:
	painter->setRenderHint(QPainter::Antialiasing);

    if (mStartPort == NULL || mEndPort == NULL || mStartPort->collidesWithItem(mEndPort))
        return;

	//ugly, only used for debug draw
	QPointF controlPoint1;
	QPointF controlPoint2;
	recreatePath(controlPoint1, controlPoint2);
	

    // TODO debug draw
    // FIXME:causes difference beteen debug draw and normal draw!!!
	//updatePosition(controlPoint1, controlPoint2);
    //if (static_cast<NodeEditorScene*>(scene())->settings.debugDraw) {
		QPen origPen = painter->pen();
		QBrush origBrush = painter->brush();
		debugPaint(painter, controlPoint1, controlPoint2);
		painter->setPen(origPen);
	    painter->setBrush(origBrush);
    //}

    QPen mPen = pen();
    mPen.setColor(mColor);
    painter->setPen(mPen);
	//line
    //painter->setBrush(mColor);
	painter->setBrush(Qt::NoBrush);

	if (isSelected())
		painter->setPen(QPen(mColor, 1, Qt::DashLine));




	//painter->drawLine(line());

	//cubic spline?
	QPainterPath& p = this->path();
	painter->drawPath(p);

	//fill
	painter->setBrush(mColor);

	//test:
    QPolygonF arrowHeadEnd = createArrowPoly(p, mEndPort);
	if (bidirectional()) {
        QPolygonF arrowHeadStart = createArrowPoly(p, mStartPort);
		p.addPolygon(arrowHeadStart);
		painter->drawPolygon(arrowHeadStart);
	}
	//do this after creating other arrow, in case it matters in arrow calc on path
	p.addPolygon(arrowHeadEnd);
	painter->drawPolygon(arrowHeadEnd);
	

	/*
	//dw new4: what was it for, now commented out
	//dw668: activated again. otherwise painting of path had hollow head
		//dw reset brush, check again
	    mPen = pen();
		mPen.setColor(mColor);
		painter->setPen(mPen);
		painter->setBrush(mColor);
        painter->drawPolygon(arrowHead);
		*/
}
	
//! [7]
