#ifndef POLYGONENTITY_H
#define POLYGONENTITY_H

#include "drawableentity.h"
#include <list>

class PolygonEntity : public DrawableEntity
{
public:
    PolygonEntity(QPoint);
    virtual ~PolygonEntity();

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
    enum ControlPoint { None, TranslateEdge, TranslatePoint };

    struct Edge {
        QPoint* first;
        QPoint* second;
    };
    struct cEdge {
        const QPoint* first;
        const QPoint* second;
    };

    double calcDistance(QPoint, QPoint) const;
    double lengthOfVect(QPoint) const;
    bool isPointOnEdge(cEdge, QPoint) const;
    ControlPoint getControlPointType(QPoint) const;

    bool m_inCreationMode;
    ControlPoint m_selectedControlType;
    QPoint* m_selectedPoint;
    Edge m_selectedEdge;

    std::list<QPoint> m_points;

    bool m_outlined;
    QTime m_outlineStartTime;
    int m_expectedTime;

    QPoint m_translateReferencePoint;
};

#endif
