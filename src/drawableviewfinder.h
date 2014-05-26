#ifndef DRAWABLEVIEWFINDER_H
#define DRAWABLEVIEWFINDER_H

#include <list>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>
#include "circleentity.h"
#include "curveentity.h"
#include "drawableentity.h"
#include "lineentity.h"
#include "polygonentity.h"
#include "rectangleentity.h"


class DrawableViewfinder : public QWidget
{
    Q_OBJECT

public:
    enum Shape { None, SinglePoint, StartPoint, EndPoint /*, Line, Rectangle */ };
    enum Entity { Pointer, Line, Rectangle, Polygon, Circle, Curve };

    DrawableViewfinder(QWidget *parent = 0);
    ~DrawableViewfinder();

    void keyPressEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

    void setEntity(Entity);
    Entity getEntity();

    bool isEntitySelected();
    void addEntity(DrawableEntity*);
    void selectEntityAtPoint(QPoint);
    void deselectEntity();
    void removeSelectedEntity();

    std::list<DrawableEntity*> *getListOfEntities(); // this is a bit sloppy isn't it?

    void setImage(QImage);
    void freezeFrame(bool);

protected:
    void paintEvent(QPaintEvent*);

signals:
    void entityChanged();
    void entityAdded();

private:
    QImage* frame;

    Entity m_mode;
    DrawableEntity* m_selectedEntity;
    std::list<DrawableEntity*> m_entities;
    QPoint m_clickPosition;

    bool m_freezeFrame;
};

#endif
