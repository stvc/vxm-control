#include "drawableviewfinder.h"

DrawableViewfinder::DrawableViewfinder(QWidget *parent) :
    QWidget(parent),
    startPoint(0,0),
    endPoint(0,0)
{
    shape = Line;
    setAutoFillBackground(false);
}

DrawableViewfinder::~DrawableViewfinder() {
}

void DrawableViewfinder::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::green, 5,
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
    }
    emit pointsChanged();
}

void DrawableViewfinder::mousePressEvent(QMouseEvent* event) {
    int x = event->x();
    int y = event->y();

    startPoint.setX(x);
    startPoint.setY(y);
    endPoint.setX(x);
    endPoint.setY(y);
    update();
}

void DrawableViewfinder::mouseReleaseEvent(QMouseEvent* event) {
    int x = event->x();
    int y = event->y();

    endPoint.setX(x);
    endPoint.setY(y);
    update();
}

void DrawableViewfinder::mouseMoveEvent(QMouseEvent* event) {
    int x = event->x();
    int y = event->y();

    endPoint.setX(x);
    endPoint.setY(y);
    update();
}

void DrawableViewfinder::setShape(Shape s) {
    shape = s;
}

QPoint DrawableViewfinder::getStartPoint() {
    return startPoint;
}

QPoint DrawableViewfinder::getEndPoint() {
    return endPoint;
}

