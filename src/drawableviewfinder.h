#ifndef DRAWABLEVIEWFINDER_H
#define DRAWABLEVIEWFINDER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>


class DrawableViewfinder : public QWidget
{
    Q_OBJECT

public:
    enum Shape { None, Point, Line, Rectangle };

    DrawableViewfinder(QWidget *parent = 0);
    ~DrawableViewfinder();

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

    void setShape(Shape);
    void resetPoints();
    QPoint getStartPoint();
    QPoint getEndPoint();

protected:
    void paintEvent(QPaintEvent*);

signals:
    void pointsChanged();

private:
    void updateEndPoint(QPoint end);
    Shape shape;
    QPoint startPoint;
    QPoint endPoint;

};

#endif
