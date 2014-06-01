#include "pointtranslator.h"

PointTranslator::PointTranslator() {
    m_xScale = 1;
    m_yScale = 1;
    m_axisSkew = 0;
    m_crosshairs = QPoint(0,0);
}

PointTranslator::PointTranslator(double xScale, double yScale, double skew, QPoint crosshairs) {
    // TODO: change to use linear transformations instead of just simply
    // scaling the X and Y axes. This will allow for rotations, as well as axis
    // inversions.
    m_xScale = xScale;
    m_yScale = yScale;
    m_axisSkew = skew;
    m_crosshairs = crosshairs;
}

PointTranslator::~PointTranslator() {
}

QPoint PointTranslator::translatePoint(QPoint p) {
    p -= m_crosshairs;
    double x = p.x() * m_xScale;
    double y = p.y() * m_yScale;

    return QPoint(x,y);
}

std::list<QPoint> PointTranslator::translatePoints(std::list<QPoint> ps) {
    std::list<QPoint> r;

    for (std::list<QPoint>::iterator it = ps.begin(); it != ps.end(); it++) {
        r.push_back(translatePoint(*it));
    }

    return r;
}
