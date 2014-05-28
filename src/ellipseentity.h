#ifndef ELLIPSEENTITY_H
#define ELLIPSEENTITY_H

#include "rectangleentity.h"

class EllipseEntity : public RectangleEntity
{
public:
    EllipseEntity(QPoint);
    virtual ~EllipseEntity();
    virtual void paintEntity(QPainter&) const;

    virtual QPoint getStartPoint() const;
    virtual std::list<std::list<QPoint> > getListOfCurves() const;
};

#endif
