#ifndef CIRCLEENTITY_H
#define CIRCLEENTITY_H

#include "drawableentity.h"

class CircleEntity : public DrawableEntity
{
public:
    CircleEntity(QPoint);
    virtual ~CircleEntity();

    virtual bool isPointOnEntity(QPoint) const;

    virtual Qt::CursorShape getMouseCursorAtPosition(QPoint) const;
    virtual void selectControlPoint(QPoint);
    virtual void moveControlPointTo(QPoint);
    virtual void deselectControlPoint();

    virtual void setOutlined(bool);
    virtual void startOutlining(int);
    virtual void paintEntity(QPainter&) const;

    virtual QPoint getStartPoint() const;
    virtual std::list<std::list<QPoint> > getListOfCurves() const;

private:
    enum ControlPoint { None, Translate, Center, Resize };

    qreal calcDistance(QPoint, QPoint) const;
    ControlPoint findControlPoint(QPoint) const;

    ControlPoint m_selectedControlPoint;
    QPoint m_centerPoint;
    int m_radius;

    bool m_outlined;
    QTime m_outlineStartTime;
    int m_expectedTime;

    QPoint m_translateReferencePoint;
};

#endif
