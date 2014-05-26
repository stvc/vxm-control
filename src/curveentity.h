#ifndef CURVEENTITY_H
#define CURVEENTITY_H

#include "drawableentity.h"

class CurveEntity : public DrawableEntity
{
public:
    CurveEntity(QPoint);
    virtual ~CurveEntity();

    virtual void setSelected(bool);
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

    void setInCreationMode(bool);

private:
    enum ControlPoint { None, TranslateAll,
        TranslateFirst, TranslateSecond, TranslateThird, TranslateForth };

    double calcDistance(QPoint, QPoint) const;
    ControlPoint findControlPoint(QPoint) const;

    QPoint m_first;
    QPoint m_second;
    QPoint m_third;
    QPoint m_forth;

    bool m_inCreationMode;
    ControlPoint m_selectedControlPoint;

    bool m_outlined;
    QTime m_outlineStartTime;
    int m_expectedTime;

    QPoint m_translateReferencePoint;
};

#endif
