#include "lineentity.h"
#include <QtCore/qmath.h>
#include <QDebug>

LineEntity::LineEntity(QPoint p) :
    m_startPoint(p),
    m_endPoint(p),
    m_translateReferencePoint(0,0)
{
    m_selectedControlPoint = None;
    m_outlineStartTime.start();
    m_expectedTime = 0;
}

LineEntity::~LineEntity() {}

bool LineEntity::isPointOnEntity(QPoint p) const {
    return (findControlPoint(p) != None);
}

Qt::CursorShape LineEntity::getMouseCursorAtPosition(QPoint p) const {
    if (findControlPoint(p) == None)
        return Qt::ArrowCursor;
    else
        return Qt::SizeAllCursor;
}

void LineEntity::selectControlPoint(QPoint p) {
    m_selectedControlPoint = findControlPoint(p);
    m_translateReferencePoint = p;
}

void LineEntity::moveControlPointTo(QPoint p) {
    if (m_selectedControlPoint == EndPoint)
        m_endPoint = p;
    else if (m_selectedControlPoint == StartPoint)
        m_startPoint = p;
    else if (m_selectedControlPoint == TranslateAll) {
        m_startPoint += (p - m_translateReferencePoint);
        m_endPoint   += (p - m_translateReferencePoint);
        m_translateReferencePoint = p;
    }
}

void LineEntity::deselectControlPoint() {
    m_selectedControlPoint = None;
}

void LineEntity::setOutlined(bool o) {
    m_outlined = o;
    m_expectedTime = 0;
}

void LineEntity::startOutlining(int expected) {
    m_outlineStartTime.start();
    m_expectedTime = expected;
}

void LineEntity::paintEntity(QPainter& p) const {
    QPoint completed = m_endPoint - m_startPoint; // use absolute point
    int elapsed = m_outlineStartTime.elapsed();
    if (elapsed < m_expectedTime) {
        double percent = elapsed / m_expectedTime;
        completed *= percent;
    }
    else if (!m_outlined) {
        completed = QPoint(0,0);
    }

    completed += m_startPoint;

    // draw line completed as green
    p.setPen(QPen(Qt::green, 2,
        Qt::PenStyle(Qt::SolidLine),
        Qt::PenCapStyle(Qt::FlatCap),
        Qt::PenJoinStyle(Qt::MiterJoin)));
    p.drawLine(m_startPoint, completed);

    if (completed != m_endPoint) {
        // draw remaining distance as red
        p.setPen(QPen(Qt::red, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawLine(completed, m_endPoint);
    }

    if (m_selected) {
        // draw control points
        p.setBrush(Qt::white);
        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawEllipse(m_startPoint, 5, 5);
        p.drawEllipse(m_endPoint, 5, 5);
        p.setBrush(Qt::NoBrush);
    }
}

QPoint LineEntity::getStartPoint() const {
    return m_startPoint;
}

std::list<std::list<QPoint> > LineEntity::getListOfCurves() const {
    std::list<std::list<QPoint> > curves;

    std::list<QPoint> c;
    c.push_back(m_startPoint);
    c.push_back(m_endPoint);
    curves.push_back(c);

    return curves;
}

qreal LineEntity::calcDistance(QPoint a, QPoint b) const {
    return qSqrt(qPow(a.x() - b.x(), 2) + qPow(a.y() - b.y(), 2));
}

LineEntity::ControlPoint LineEntity::findControlPoint(QPoint p) const {
    if (calcDistance(p, m_endPoint) < 5.0)
        return EndPoint;
    if (calcDistance(p, m_startPoint) < 5.0)
        return StartPoint;

    QPoint vect = m_endPoint - m_startPoint;
    qreal angle = qAtan2(vect.x(), vect.y());
    qreal xdelta = qSin(angle);
    qreal ydelta = qCos(angle);

    QPoint delta = m_startPoint;
    int i = 1;
    while (!(calcDistance(m_endPoint, delta) < 5.0)) {
        delta = QPoint(3.0 * i * xdelta, 3.0 * i * ydelta);
        delta += m_startPoint;
        if (calcDistance(p,delta) < 5.0)
            return TranslateAll;
        i++;
    }
    return None;
}

