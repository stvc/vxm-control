#ifndef DRAWABLEENTITY_H
#define DRAWABLEENTITY_H

#include <list>
#include <QPainter>
#include <QPoint>
#include <QTime>


/**
 * It is assumed that each entity is continuous, so the method
 * getListOfCurves() should return a list of lists of control points, with the
 * last point in each list matching the first point in the next list
 */
class DrawableEntity
{
public:
    DrawableEntity() : CONTROL_POINT_RADIUS(2) { m_selected = false; };
    virtual ~DrawableEntity() {};

    virtual void setSelected(bool s) { m_selected = s; };
    virtual bool isSelected() const { return m_selected; };
    virtual bool isPointOnEntity(QPoint) const = 0;

    virtual Qt::CursorShape getMouseCursorAtPosition(QPoint) const = 0;
    virtual void selectControlPoint(QPoint) = 0;
    virtual void moveControlPointTo(QPoint) = 0;
    virtual void deselectControlPoint() = 0;

    virtual void setOutlined(bool) = 0;
    virtual void startOutlining(int) = 0;
    virtual void paintEntity(QPainter&) const = 0;

    virtual QPoint getStartPoint() const = 0;

    virtual std::list<std::list<QPoint> > getListOfCurves() const = 0;

protected:
    int const CONTROL_POINT_RADIUS;
    bool m_selected;
};


#endif
