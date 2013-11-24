#ifndef DRAWABLEVIEWFINDER_H
#define DRAWABLEVIEWFINDER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>


class DrawableViewfinder : public QWidget
{
    Q_OBJECT

public:
    enum Shape { Line, Rectangle };

    DrawableViewfinder(QWidget *parent = 0);
    ~DrawableViewfinder();

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

    void setShape(Shape);
    QPoint getStartPoint();
    QPoint getEndPoint();

protected:
    void paintEvent(QPaintEvent*);

signals:
    void pointsChanged();

private:
    Shape shape;
    QPoint startPoint;
    QPoint endPoint;

};

#endif
