#include "rectangleentity.h"
#include <QtCore/qmath.h>
#include <QDebug>
#include <QVector>

RectangleEntity::RectangleEntity(QPoint p) :
    m_startPoint(p),
    m_endPoint(p),
    m_translateReferencePoint(0,0)
{
    m_selectedControlPoint = None;
    m_outlineStartTime.start();
    m_expectedTime = 0;
    m_outlined = true;
}

RectangleEntity::~RectangleEntity() {
}

bool RectangleEntity::isPointOnEntity(QPoint p) const {
    return (findControlPoint(p) != None);
}

Qt::CursorShape RectangleEntity::getMouseCursorAtPosition(QPoint p) const {
    // this method shouldn't be called if entity isn't selected, but just in
    // case:
    if (!m_selected)
        return Qt::ArrowCursor;

    // return correct cursor corresponding to control point
    ControlPoint cp = findControlPoint(p);
    switch (cp) {
        case TopLeftCorner:
        case BottomRightCorner:
            return Qt::SizeFDiagCursor;
        case TopRightCorner:
        case BottomLeftCorner:
            return Qt::SizeBDiagCursor;
        case TopEdge:
        case BottomEdge:
            return Qt::SizeVerCursor;
        case LeftEdge:
        case RightEdge:
            return Qt::SizeHorCursor;
        case TranslateAll:
            return Qt::SizeAllCursor;
        default:
            break;
    }

    // if not over a control point:
    return Qt::ArrowCursor;
}

void RectangleEntity::selectControlPoint(QPoint p) {
    m_selectedControlPoint = findControlPoint(p);
    m_translateReferencePoint = p;
}

void RectangleEntity::moveControlPointTo(QPoint p) {
    switch (m_selectedControlPoint) {
        case TopLeftCorner:
            m_startPoint = p;
            break;
        case BottomRightCorner:
            m_endPoint = p;
            break;
        case TopRightCorner:
            m_startPoint.setY(p.y());
            m_endPoint.setX(p.x());
            break;
        case BottomLeftCorner:
            m_startPoint.setX(p.x());
            m_endPoint.setY(p.y());
            break;
        case TopEdge:
            m_startPoint.setY(p.y());
            break;
        case BottomEdge:
            m_endPoint.setY(p.y());
            break;
        case LeftEdge:
            m_startPoint.setX(p.x());
            break;
        case RightEdge:
            m_endPoint.setX(p.x());
            break;
        case TranslateAll:
            m_startPoint += (p - m_translateReferencePoint);
            m_endPoint += (p - m_translateReferencePoint);
            m_translateReferencePoint = p;
            break;
        default:
            break;
    }
}

void RectangleEntity::deselectControlPoint() {
    // make sure that m_startPoint is in the top left and m_endPoint is in
    // bottom right
    normalizeCorners();
    m_selectedControlPoint = None;
}

void RectangleEntity::setOutlined(bool o) {
    m_outlined = o;
    m_expectedTime = 0;
}

void RectangleEntity::startOutlining(int expected) {
    m_outlineStartTime.start();
    m_expectedTime = expected;
}

void RectangleEntity::paintEntity(QPainter& p) const {
    int elapsed = m_outlineStartTime.elapsed();
    if (m_outlined || elapsed > m_expectedTime) {
        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawRect(QRect(m_startPoint, m_endPoint));
    }
    else if (!m_outlined && m_expectedTime == 0) {
        p.setPen(QPen(Qt::red, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawRect(QRect(m_startPoint, m_endPoint));
    }
    else {
        double percentCompleted = (double) elapsed / m_expectedTime;
        int perimeterOfRect = 2 * (m_endPoint.x() - m_startPoint.x()) + 2 * (m_endPoint.y() - m_startPoint.y());
        if (perimeterOfRect < 0)
            perimeterOfRect *= -1;
        int perimeterCompleted = qFloor(perimeterOfRect * percentCompleted + 0.5);

        QVector<QPoint> completed;
        QVector<QPoint> uncompleted;

        QPoint points[5];
        points[0] = m_startPoint;
        points[1] = QPoint(m_endPoint.x(), m_startPoint.y());
        points[2] = m_endPoint;
        points[3] = QPoint(m_startPoint.x(), m_endPoint.y());
        points[4] = m_startPoint;

        double completedLength = 0;
        for (int i=1; i<5; i++) {
            if (perimeterCompleted > 0) {
                completed.push_back(points[i-1]);
                double l = calcDistance(points[i-1], points[i]);
                if (l < perimeterCompleted) {
                    // draw completed line
                    perimeterCompleted -= l;
                    completed.push_back(points[i]);
                }
                else {
                    // subdivide line
                    double factor = perimeterCompleted / l;
                    perimeterCompleted = 0;
                    QPoint midPoint = (points[i] - points[i-1]) * factor + points[i-1];
                    completed.push_back(midPoint);
                    uncompleted.push_back(midPoint);
                    uncompleted.push_back(points[i]);
                }
            }
            else {
                uncompleted.push_back(points[i-1]);
                uncompleted.push_back(points[i]);
            }
        }

        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawLines(completed);

        p.setPen(QPen(Qt::red, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawLines(uncompleted);
    }

    if (m_selected) {
        // point is selected, draw control points
        p.setBrush(Qt::white);
        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.drawEllipse(m_startPoint, 5, 5);
        p.drawEllipse(QPoint(m_endPoint.x(), m_startPoint.y()), 5, 5);
        p.drawEllipse(m_endPoint, 5, 5);
        p.drawEllipse(QPoint(m_startPoint.x(), m_endPoint.y()), 5, 5);

        QPoint topMid((m_endPoint.x() + m_startPoint.x())/2, m_startPoint.y());
        QPoint botMid((m_endPoint.x() + m_startPoint.x())/2, m_endPoint.y());
        QPoint leftMid(m_startPoint.x(), (m_endPoint.y() + m_startPoint.y())/2);
        QPoint rightMid(m_endPoint.x(), (m_endPoint.y() + m_startPoint.y())/2);
        QPoint delta(2,2);

        p.drawRect(QRect(topMid - delta, topMid + delta));
        p.drawRect(QRect(botMid - delta, botMid + delta));
        p.drawRect(QRect(leftMid - delta, leftMid + delta));
        p.drawRect(QRect(rightMid - delta, rightMid + delta));
        p.setBrush(Qt::NoBrush);

    }
}

QPoint RectangleEntity::getStartPoint() const {
    return m_startPoint;
}

std::list<std::list<QPoint> > RectangleEntity::getListOfCurves() const {
    std::list<std::list<QPoint> > curves;

    std::list<QPoint> c;
    c.push_back(m_startPoint);
    c.push_back(QPoint(m_endPoint.x(), m_startPoint.y()));
    curves.push_back(c);

    c.clear();
    c.push_back(QPoint(m_endPoint.x(), m_startPoint.y()));
    c.push_back(m_endPoint);
    curves.push_back(c);

    c.clear();
    c.push_back(m_endPoint);
    c.push_back(QPoint(m_startPoint.x(), m_endPoint.y()));
    curves.push_back(c);

    c.clear();
    c.push_back(QPoint(m_startPoint.x(), m_endPoint.y()));
    c.push_back(m_startPoint);
    curves.push_back(c);

    return curves;
}

qreal RectangleEntity::calcDistance(QPoint a, QPoint b) const {
    return qSqrt(qPow(a.x() - b.x(), 2) + qPow(a.y() - b.y(), 2));
}

void RectangleEntity::normalizeCorners() {
    if (m_startPoint.x() > m_endPoint.x()) {
        int tmp = m_startPoint.x();
        m_startPoint.setX(m_endPoint.x());
        m_endPoint.setX(tmp);
    }
    if (m_startPoint.y() > m_endPoint.y()) {
        int tmp = m_startPoint.y();
        m_startPoint.setY(m_endPoint.y());
        m_endPoint.setY(tmp);
    }
}

RectangleEntity::ControlPoint RectangleEntity::findControlPoint(QPoint p) const {
    // top left corner
    if (calcDistance(p, m_startPoint) < 5.0) {
        return TopLeftCorner;
    }

    // top right corner
    if (calcDistance(p, QPoint(m_endPoint.x(), m_startPoint.y())) < 5.0) {
        return TopRightCorner;
    }

    // bottom left corner
    if (calcDistance(p, QPoint(m_startPoint.x(), m_endPoint.y())) < 5.0) {
        return BottomLeftCorner;
    }

    //
    // bottom right corner
    if (calcDistance(p, m_endPoint) < 5.0) {
        return BottomRightCorner;
    }

    // top edge
    if (p.y() > m_startPoint.y() - 5 && p.y() < m_startPoint.y() + 5
            && p.x() > m_startPoint.x() && p.x() < m_endPoint.x()) {
        if (calcDistance(p, QPoint((m_endPoint.x() + m_startPoint.x())/2, m_startPoint.y())) < 5.0)
            return TopEdge;
        else
            return TranslateAll;
    }

    // bottom edge
    if (p.y() > m_endPoint.y() - 5 && p.y() < m_endPoint.y() + 5
            && p.x() > m_startPoint.x() && p.x() < m_endPoint.x()) {
        if (calcDistance(p, QPoint((m_startPoint.x() + m_endPoint.x())/2, m_endPoint.y())) < 5.0)
            return BottomEdge;
        else
            return TranslateAll;
    }

    // left edge
    if (p.x() > m_startPoint.x() - 5 && p.x() < m_startPoint.x() + 5
            && p.y() > m_startPoint.y() && p.y() < m_endPoint.y()) {
        if (calcDistance(p, QPoint(m_startPoint.x(), (m_startPoint.y() + m_endPoint.y())/2)) < 5.0)
            return LeftEdge;
        else
            return TranslateAll;
    }
    // right edge
    if (p.x() > m_endPoint.x() - 5 && p.x() < m_endPoint.x() + 5
            && p.y() > m_startPoint.y() && p.y() < m_endPoint.y()) {
        if (calcDistance(p, QPoint(m_endPoint.x(), (m_startPoint.y() + m_endPoint.y())/2)) < 5.0)
            return RightEdge;
        else
            return TranslateAll;
    }

    return None;
}

