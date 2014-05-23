#ifndef POINTTRANSLATOR_H
#define POINTTRANSLATOR_H

#include <list>
#include <QPoint>

class PointTranslator {

public:
    PointTranslator();
    PointTranslator(double, double, double, QPoint);
    ~PointTranslator();

    QPoint translatePoint(QPoint);
    std::list<QPoint> translatePoints(std::list<QPoint>);

private:
    QPoint m_crosshairs;
    double m_xScale;
    double m_yScale;
    double m_axisSkew;
};

#endif
