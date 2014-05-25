#include "circleentity.h"
#include <QtCore/qmath.h>
#include <QDebug>
#include <QRect>

CircleEntity::CircleEntity(QPoint p) :
    m_centerPoint(p),
    m_translateReferencePoint(0,0)
{
    m_selectedControlPoint = None;
    m_outlineStartTime.start();
    m_outlined = true;
    m_expectedTime = 0;
    m_radius = 0;
}

CircleEntity::~CircleEntity() {}

bool CircleEntity::isPointOnEntity(QPoint p) const {
    ControlPoint cp = findControlPoint(p);
    if (m_selected)
        return (cp != None);
    else
        return (cp == Resize || cp == Center);
}

Qt::CursorShape CircleEntity::getMouseCursorAtPosition(QPoint p) const {
    if (findControlPoint(p) == None)
        return Qt::ArrowCursor;
    else
        return Qt::SizeAllCursor;
}

void CircleEntity::selectControlPoint(QPoint p) {
    m_selectedControlPoint = findControlPoint(p);
    m_translateReferencePoint = p;
}

void CircleEntity::moveControlPointTo(QPoint p) {
    if (m_selectedControlPoint == Resize) {
        m_radius = qFloor(calcDistance(m_centerPoint, p) + 0.5);
    }
    else if (m_selectedControlPoint == Translate || m_selectedControlPoint == Center) {
        m_centerPoint += (p - m_translateReferencePoint);
        m_translateReferencePoint = p;
    }
}

void CircleEntity::deselectControlPoint() {
    m_selectedControlPoint = None;
}

void CircleEntity::setOutlined(bool o) {
    m_outlined = o;
    m_expectedTime = 0;
}

void CircleEntity::startOutlining(int expected) {
    m_outlineStartTime.start();
    m_expectedTime = expected;
}

void CircleEntity::paintEntity(QPainter& p) const {

    QRect circ(m_centerPoint.x() - m_radius, m_centerPoint.y() - m_radius, m_radius * 2, m_radius * 2);
    int startAngle = 90 * 16;

    double percentComplete = 0;
    if (m_outlined) {
        percentComplete = 1;
    }
    else {
        int elapsed = m_outlineStartTime.elapsed();
        if (elapsed < m_expectedTime) {
            percentComplete = elapsed / m_expectedTime;
        }
    }

    int spanAngle = qFloor((-360 * 16 * percentComplete) + 90.5);


    p.setPen(QPen(Qt::red, 2,
        Qt::PenStyle(Qt::SolidLine),
        Qt::PenCapStyle(Qt::FlatCap),
        Qt::PenJoinStyle(Qt::MiterJoin)));

    if (percentComplete == 0) {
        p.drawEllipse(circ);
    }
    else if (percentComplete != 1) {
        p.drawArc(circ, startAngle, spanAngle);
    }

    p.setPen(QPen(Qt::green, 2,
        Qt::PenStyle(Qt::SolidLine),
        Qt::PenCapStyle(Qt::FlatCap),
        Qt::PenJoinStyle(Qt::MiterJoin)));

    if (percentComplete == 1) {
        p.drawEllipse(circ);
    }
    else if (percentComplete != 0) {
        p.drawArc(circ, spanAngle, startAngle - 360*16);
    }

    if (m_selected) {
        p.setBrush(Qt::white);
        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawEllipse(m_centerPoint, 5, 5);

        QPoint delta(2,2);
        QPoint cp(m_centerPoint.x() + m_radius, m_centerPoint.y());
        p.drawRect(QRect(cp - delta, cp + delta));
        p.setBrush(Qt::NoBrush);
    }
}

QPoint CircleEntity::getStartPoint() const {
    return QPoint(m_centerPoint.x(), m_centerPoint.y() - m_radius);
}

std::list<std::list<QPoint> > CircleEntity::getListOfCurves() const {
    std::list<std::list<QPoint> > curves;

    int magicNum = qFloor(m_radius * 0.55228 + 0.5);

    std::list<QPoint> c;
    c.push_back(QPoint(m_centerPoint.x(), m_centerPoint.y() - m_radius));
    c.push_back(QPoint(m_centerPoint.x() + magicNum, m_centerPoint.y() - m_radius));
    c.push_back(QPoint(m_centerPoint.x() + m_radius, m_centerPoint.y() - magicNum));
    c.push_back(QPoint(m_centerPoint.x() + m_radius, m_centerPoint.y()));
    curves.push_back(c);

    c.clear();
    c.push_back(QPoint(m_centerPoint.x() + m_radius, m_centerPoint.y()));
    c.push_back(QPoint(m_centerPoint.x() + m_radius, m_centerPoint.y() + magicNum));
    c.push_back(QPoint(m_centerPoint.x() + magicNum, m_centerPoint.y() + m_radius));
    c.push_back(QPoint(m_centerPoint.x(), m_centerPoint.y() + m_radius));
    curves.push_back(c);

    c.clear();
    c.push_back(QPoint(m_centerPoint.x(), m_centerPoint.y() + m_radius));
    c.push_back(QPoint(m_centerPoint.x() - magicNum, m_centerPoint.y() + m_radius));
    c.push_back(QPoint(m_centerPoint.x() - m_radius, m_centerPoint.y() + magicNum));
    c.push_back(QPoint(m_centerPoint.x() - m_radius, m_centerPoint.y()));
    curves.push_back(c);

    c.clear();
    c.push_back(QPoint(m_centerPoint.x() - m_radius, m_centerPoint.y()));
    c.push_back(QPoint(m_centerPoint.x() - m_radius, m_centerPoint.y() - magicNum));
    c.push_back(QPoint(m_centerPoint.x() - magicNum, m_centerPoint.y() - m_radius));
    c.push_back(QPoint(m_centerPoint.x(), m_centerPoint.y() - m_radius));
    curves.push_back(c);

    return curves;
}

qreal CircleEntity::calcDistance(QPoint a, QPoint b) const {
    return qSqrt(qPow(a.x() - b.x(), 2) + qPow(a.y() - b.y(), 2));
}

CircleEntity::ControlPoint CircleEntity::findControlPoint(QPoint p) const {
    qreal dist = calcDistance(p,m_centerPoint);
    if (dist < (m_radius + 5) && dist > m_radius - 5) {
        return Resize;
    }
    else if (dist < 5) {
        return Center;
    }
    else if (dist < m_radius) {
        return Translate;
    }
    return None;
}

