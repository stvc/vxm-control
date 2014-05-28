#ifndef RECTANGLEENTITY_H
#define RECTANGLEENTITY_H

#include "drawableentity.h"

class RectangleEntity : public DrawableEntity
{
public:
    RectangleEntity(QPoint);
    virtual ~RectangleEntity();

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

protected:
    enum ControlPoint { None, TranslateAll,
                        TopRightCorner, TopLeftCorner, BottomRightCorner, BottomLeftCorner,
                        LeftEdge, TopEdge, RightEdge, BottomEdge };

    qreal calcDistance(QPoint, QPoint) const;
    void normalizeCorners();
    ControlPoint findControlPoint(QPoint) const;

    ControlPoint m_selectedControlPoint;
    QPoint m_startPoint;
    QPoint m_endPoint;

    bool m_outlined;
    QTime m_outlineStartTime;
    int m_expectedTime;

    QPoint m_translateReferencePoint;
};

#endif
