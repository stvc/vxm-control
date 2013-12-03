#include "drawableviewfinder.h"

DrawableViewfinder::DrawableViewfinder(QWidget *parent) :
    QWidget(parent),
    startPoint(0,0),
    endPoint(0,0)
{
    shape = None;
    pointsFrozen = false;
    setAutoFillBackground(false);
}

DrawableViewfinder::~DrawableViewfinder() {
}

void DrawableViewfinder::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::green, 2,
                Qt::PenStyle(Qt::SolidLine),
                Qt::PenCapStyle(Qt::FlatCap),
                Qt::PenJoinStyle(Qt::MiterJoin)));
    painter.setBackground(Qt::NoBrush);
    switch (shape) {
        case Line:
            painter.drawLine(QLine(startPoint, endPoint));
            break;
        case Rectangle:
            painter.drawRect(QRect(startPoint, endPoint));
            break;
        case SinglePoint:
            painter.drawPoint(endPoint);
            break;
        case StartPoint:
            painter.drawPoint(startPoint);
            break;
        case EndPoint:
            painter.drawPoint(startPoint);
            painter.drawPoint(endPoint);
            break;
        default:
            break;
    }
}

void DrawableViewfinder::mousePressEvent(QMouseEvent* event) {
    if (shape == None || pointsFrozen) {
        return;
    }
    int x = event->x();
    int y = event->y();

    startPoint = QPoint(x,y);
    endPoint = QPoint(x,y);
    update();
}

void DrawableViewfinder::mouseReleaseEvent(QMouseEvent* event) {
    if (shape == None || pointsFrozen) {
        return;
    }
    int x = event->x();
    int y = event->y();

    updateEndPoint(QPoint(x,y));
    update();
}

void DrawableViewfinder::mouseMoveEvent(QMouseEvent* event) {
    if (shape == None || pointsFrozen) {
        return;
    }
    int x = event->x();
    int y = event->y();

    updateEndPoint(QPoint(x,y));
    update();
}

void DrawableViewfinder::setShape(Shape s) {
    shape = s;
    update();
}

DrawableViewfinder::Shape DrawableViewfinder::getShape() {
    return shape;
}

QPoint DrawableViewfinder::getStartPoint() {
    return startPoint;
}

QPoint DrawableViewfinder::getEndPoint() {
    return endPoint;
}

void DrawableViewfinder::setStartPoint(QPoint p) {
    startPoint = p;
}

void DrawableViewfinder::resetPoints() {
    startPoint = QPoint(0,0);
    endPoint = QPoint(0,0);
    update();
}

void DrawableViewfinder::freezePoints(bool f) {
    pointsFrozen = f;
}

void DrawableViewfinder::updateEndPoint(QPoint end) {
    if (shape == StartPoint || shape == SinglePoint) {
        endPoint = end;
        startPoint = end;
    }
    if (shape == EndPoint) {
        endPoint = end;
    }
    else if (shape == Line) {
        int xdiff = end.x() - startPoint.x();
        int ydiff = end.y() - startPoint.y();
        if (xdiff < 0) xdiff *= -1;
        if (ydiff < 0) ydiff *= -1;
        if (xdiff > ydiff) { // draw horizontal line
            endPoint = QPoint(end.x(), startPoint.y());
        }
        else {
            endPoint = QPoint(startPoint.x(), end.y());
        }
    }
    else if (shape == Rectangle) {
        endPoint = end;
    }
    emit pointsChanged();
}
