#include "ellipseentity.h"
#include <QtCore/qmath.h>
#include <QDebug>
#include <QVector>

EllipseEntity::EllipseEntity(QPoint p) : RectangleEntity(p) {
}

EllipseEntity::~EllipseEntity() {}

void EllipseEntity::paintEntity(QPainter& p) const {
    QRect circ(m_startPoint, m_endPoint);
    if (m_outlined || m_outlineStartTime.elapsed() > m_expectedTime) {
        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawEllipse(circ);
    }
    else if (!m_outlined && m_expectedTime == 0) {
        p.setPen(QPen(Qt::red, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawEllipse(circ);
    }
    else {
        double percentComplete = (double) m_outlineStartTime.elapsed() / m_expectedTime;
        int completedAngle = qFloor((double) -360 * 16 * percentComplete + 0.5);
        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawArc(circ, 90 * 16, completedAngle);

        p.setPen(QPen(Qt::red, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawArc(circ, completedAngle + (90 * 16), (-360*16) - completedAngle);
    }

    if (m_selected) {
        // point is selected, draw control points
        p.setPen(QPen(Qt::black, 1,
            Qt::PenStyle(Qt::DotLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawRect(QRect(m_startPoint, m_endPoint));

        p.setBrush(Qt::white);
        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));

        p.drawEllipse(m_startPoint, CONTROL_POINT_RADIUS * 2, CONTROL_POINT_RADIUS * 2);
        p.drawEllipse(QPoint(m_endPoint.x(), m_startPoint.y()), CONTROL_POINT_RADIUS * 2, CONTROL_POINT_RADIUS * 2);
        p.drawEllipse(m_endPoint, CONTROL_POINT_RADIUS * 2, CONTROL_POINT_RADIUS * 2);
        p.drawEllipse(QPoint(m_startPoint.x(), m_endPoint.y()), CONTROL_POINT_RADIUS * 2, CONTROL_POINT_RADIUS * 2);

        QPoint topMid((m_endPoint.x() + m_startPoint.x())/2, m_startPoint.y());
        QPoint botMid((m_endPoint.x() + m_startPoint.x())/2, m_endPoint.y());
        QPoint leftMid(m_startPoint.x(), (m_endPoint.y() + m_startPoint.y())/2);
        QPoint rightMid(m_endPoint.x(), (m_endPoint.y() + m_startPoint.y())/2);
        QPoint delta(CONTROL_POINT_RADIUS, CONTROL_POINT_RADIUS);

        p.drawRect(QRect(topMid - delta, topMid + delta));
        p.drawRect(QRect(botMid - delta, botMid + delta));
        p.drawRect(QRect(leftMid - delta, leftMid + delta));
        p.drawRect(QRect(rightMid - delta, rightMid + delta));
        p.setBrush(Qt::NoBrush);

    }
}

QPoint EllipseEntity::getStartPoint() const {
    return QPoint((m_endPoint.x() + m_startPoint.x()) / 2, m_startPoint.y());
}

std::list<std::list<QPoint> > EllipseEntity::getListOfCurves() const {
    std::list<std::list<QPoint> > curves;

    double xRadius = (double) (m_endPoint.x() - m_startPoint.x()) / 2;
    double yRadius = (double) (m_endPoint.y() - m_startPoint.y()) / 2;
    int magicXNum = qFloor(xRadius * 0.55228 + 0.5);
    int magicYNum = qFloor(yRadius * 0.55228 + 0.5);

    std::list<QPoint> c;
    c.push_back(QPoint(m_startPoint.x() + xRadius, m_startPoint.y()));
    c.push_back(QPoint(m_startPoint.x() + xRadius + magicXNum, m_startPoint.y()));
    c.push_back(QPoint(m_endPoint.x(), m_endPoint.y() - yRadius - magicYNum));
    c.push_back(QPoint(m_endPoint.x(), m_endPoint.y() - yRadius));
    curves.push_back(c);

    c.clear();
    c.push_back(QPoint(m_endPoint.x(), m_endPoint.y() - yRadius));
    c.push_back(QPoint(m_endPoint.x(), m_endPoint.y() - yRadius + magicYNum));
    c.push_back(QPoint(m_endPoint.x() - xRadius + magicXNum, m_endPoint.y()));
    c.push_back(QPoint(m_endPoint.x() - xRadius, m_endPoint.y()));
    curves.push_back(c);

    c.clear();
    c.push_back(QPoint(m_endPoint.x() - xRadius, m_endPoint.y()));
    c.push_back(QPoint(m_endPoint.x() - xRadius - magicXNum, m_endPoint.y()));
    c.push_back(QPoint(m_startPoint.x(), m_startPoint.y() + yRadius + magicYNum));
    c.push_back(QPoint(m_startPoint.x(), m_startPoint.y() + yRadius));
    curves.push_back(c);

    c.clear();
    c.push_back(QPoint(m_startPoint.x(), m_startPoint.y() + yRadius));
    c.push_back(QPoint(m_startPoint.x(), m_startPoint.y() + yRadius - magicYNum));
    c.push_back(QPoint(m_startPoint.x() + xRadius - magicXNum, m_startPoint.y()));
    c.push_back(QPoint(m_startPoint.x() + xRadius, m_startPoint.y()));
    curves.push_back(c);

    return curves;
}

