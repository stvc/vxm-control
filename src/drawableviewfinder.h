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
    enum Entity { Pointer, CalibrationMode, Line, Rectangle, Polygon, Circle, Curve };

    DrawableViewfinder(QWidget *parent = 0);
    ~DrawableViewfinder();

    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
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

    QPoint getClickPosition();
    std::list<DrawableEntity*> *getListOfEntities(); // this is a bit sloppy isn't it?

    void setImage(QImage);
    void freezeFrame(bool);

protected:
    void paintEvent(QPaintEvent*);

signals:
    void entityChanged();
    void entityAdded();
    void pointClicked();

private:
    QImage* frame;

    Entity m_mode;
    DrawableEntity* m_selectedEntity;
    std::list<DrawableEntity*> m_entities;

    QPoint m_clickPosition;

    bool m_freezeFrame;
};

#endif
