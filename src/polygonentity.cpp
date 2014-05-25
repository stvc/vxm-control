#include "polygonentity.h"
#include <QtCore/qmath.h>
#include <QDebug>
#include <QRect>

PolygonEntity::PolygonEntity(QPoint p) :
    m_translateReferencePoint(0,0)
{
    m_selectedControlType = None;
    m_outlineStartTime.start();
    m_expectedTime = 0;
    m_outlined = true;

    m_inCreationMode = true;

    m_points.push_back(p);

    m_selectedPoint = NULL;
    m_selectedEdge.first = NULL;
    m_selectedEdge.second = NULL;
}

PolygonEntity::~PolygonEntity() {}

void PolygonEntity::setSelected(bool s) {
    m_selected = s;
    if (!s) {
        m_inCreationMode = false;
    }
}

bool PolygonEntity::isPointOnEntity(QPoint p) const {

    if (m_inCreationMode) {
        return true;
    }

    // TODO: refactor this to only iterate through list of points once

    // test if point is on point
    for (std::list<QPoint>::const_iterator it = m_points.begin(); it != m_points.end(); it++) {
        if (calcDistance((*it), p) < 5.0) {
            return true;
        }
    }

    // test if point is on edge
    if (m_points.size() > 1) {
        std::list<QPoint>::const_iterator fst = m_points.begin();
        std::list<QPoint>::const_iterator snd = m_points.begin();
        snd++;

        PolygonEntity::cEdge e;
        while (snd != m_points.end()) {
            e.first = &(*fst);
            e.second = &(*snd);
            if (isPointOnEdge(e, p)) {
                return true;
            }
            fst++;
            snd++;
        }
    }

    return false;
}

Qt::CursorShape PolygonEntity::getMouseCursorAtPosition(QPoint /* p */) const {
    // TODO: maybe this method needs work
    return Qt::ArrowCursor;
}

void PolygonEntity::selectControlPoint(QPoint p) {
    if (m_inCreationMode) {
        // check if new point is close to existing point, if it is, select that
        // one as the current control point
        for (std::list<QPoint>::iterator it = m_points.begin(); it != m_points.end(); it++) {
            if (calcDistance(p, *it) < 5.0) {
                m_selectedPoint = &(*it);
                m_selectedControlType = TranslatePoint;
                m_translateReferencePoint = p;
                return;
            }
        }

        // if it isn't an existing point, add that point and set is as selected
        m_points.push_back(p);
        m_selectedPoint = &(m_points.back());
        m_selectedControlType = TranslatePoint;
        m_translateReferencePoint = p;
        return;
    }
    else {
        for (std::list<QPoint>::iterator it = m_points.begin(); it != m_points.end(); it++) {
            if (calcDistance((*it), p) < 5.0) {
                m_selectedPoint = &(*it);
                m_selectedControlType = TranslatePoint;
                m_translateReferencePoint = p;
                return;
            }
        }
        // test if point is on edge
        if (m_points.size() > 1) {
            std::list<QPoint>::iterator fst = m_points.begin();
            std::list<QPoint>::iterator snd = m_points.begin();
            snd++;

            PolygonEntity::cEdge e;
            while (snd != m_points.end()) {
                e.first = &(*fst);
                e.second = &(*snd);
                if (isPointOnEdge(e, p)) {
                    m_selectedEdge.first = &(*fst);
                    m_selectedEdge.second = &(*snd);
                    m_selectedControlType = TranslateEdge;
                    m_translateReferencePoint = p;
                    return;
                }
                fst++;
                snd++;
            }
        }
    }
    m_translateReferencePoint = p;
    m_selectedControlType = None;
}

void PolygonEntity::moveControlPointTo(QPoint p) {
    if (m_selectedControlType == TranslatePoint) {
        m_selectedPoint->setX(p.x());
        m_selectedPoint->setY(p.y());
    }
    else if (m_selectedControlType == TranslateEdge) {
        QPoint delta = p - m_translateReferencePoint;
        m_selectedEdge.first->setX(m_selectedEdge.first->x() + delta.x());
        m_selectedEdge.first->setY(m_selectedEdge.first->y() + delta.y());
        m_selectedEdge.second->setX(m_selectedEdge.second->x() + delta.x());
        m_selectedEdge.second->setY(m_selectedEdge.second->y() + delta.y());
        m_translateReferencePoint = p;
    }
}

void PolygonEntity::deselectControlPoint() {
    m_selectedControlType = None;
}

void PolygonEntity::setOutlined(bool o) {
    m_outlined = o;
    m_expectedTime = 0;
}

void PolygonEntity::startOutlining(int expected) {
    m_outlineStartTime.start();
    m_expectedTime = expected;
}

void PolygonEntity::paintEntity(QPainter& p) const {

    // TODO: redo this method for progress indicator
    p.setPen(QPen(Qt::green, 2,
        Qt::PenStyle(Qt::SolidLine),
        Qt::PenCapStyle(Qt::FlatCap),
        Qt::PenJoinStyle(Qt::MiterJoin)));

    if (m_points.size() > 1) {
        std::list<QPoint>::const_iterator fst = m_points.begin();
        std::list<QPoint>::const_iterator snd = m_points.begin();
        snd++;

        while (snd != m_points.end()) {
            p.drawLine(*fst, *snd);
            fst++;
            snd++;
        }
    }

    if (m_selected) {
        p.setBrush(Qt::white);
        QPoint border(2,2);
        for (std::list<QPoint>::const_iterator it = m_points.begin(); it != m_points.end(); it++) {
            p.drawEllipse(*it, 5, 5);
        }
        p.setBrush(Qt::NoBrush);
    }

    // TODO: delete this
    /*

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

    int spanAngle = qFloor((-360 * 16 * percentComplete) + 89.5);


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
    */
}

QPoint PolygonEntity::getStartPoint() const {
    return m_points.front();
}

std::list<std::list<QPoint> > PolygonEntity::getListOfCurves() const {
    std::list<std::list<QPoint> > curves;

    std::list<QPoint>::const_iterator fst = m_points.begin();
    std::list<QPoint>::const_iterator snd = m_points.begin();
    snd++;

    while (snd != m_points.end()) {
        std::list<QPoint> c;
        c.push_back(*fst);
        c.push_back(*snd);
        curves.push_back(c);

        fst++;
        snd++;
    }

    return curves;
}

void PolygonEntity::setInCreationMode(bool b) {
    m_inCreationMode = b;
}

double PolygonEntity::calcDistance(QPoint a, QPoint b) const {
    return qSqrt(qPow(a.x() - b.x(), 2) + qPow(a.y() - b.y(), 2));
}

double PolygonEntity::lengthOfVect(QPoint p) const {
    return qSqrt(qPow(p.x(), 2) + qPow(p.y(), 2));
}

bool PolygonEntity::isPointOnEdge(cEdge edge, QPoint p) const {
    QPoint vect = *edge.second - *edge.first;

    int iterations = qFloor(lengthOfVect(vect) / 2 + 0.5);
    for (int i=0; i<iterations; i++) {
        QPoint testPoint = vect * ((double) i / iterations) + *edge.first;
        if (calcDistance(p, testPoint) < 5.0) {
            return true;
        }
    }
    return false;
}

