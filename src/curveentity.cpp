#include "curveentity.h"
#include <QtCore/qmath.h>
#include <QDebug>
#include <QRect>

CurveEntity::CurveEntity(QPoint p) :
    m_translateReferencePoint(0,0)
{
    m_selectedControlPoint = None;
    m_outlineStartTime.start();
    m_expectedTime = 0;
    m_outlined = true;

    m_inCreationMode = true;

    m_first  = p;
    m_second = p;
    m_third  = p;
    m_forth  = p;
}

CurveEntity::~CurveEntity() {}

void CurveEntity::setSelected(bool s) {
    m_selected = s;
    if (!s) {
        m_inCreationMode = false;
    }
}

bool CurveEntity::isPointOnEntity(QPoint p) const {
    CurveEntity::ControlPoint cp = findControlPoint(p);
    if (!m_selected)
        return (cp == TranslateAll || cp == TranslateFirst || cp == TranslateForth);
    else
        return (cp != None);
}

Qt::CursorShape CurveEntity::getMouseCursorAtPosition(QPoint /* p */) const {
    // TODO: this method needs work
    return Qt::ArrowCursor;
}

void CurveEntity::selectControlPoint(QPoint p) {
    m_selectedControlPoint = findControlPoint(p);
    m_translateReferencePoint = p;
}

void CurveEntity::moveControlPointTo(QPoint p) {
    if (m_selectedControlPoint == TranslateFirst) {
        m_first = p;
    }
    else if (m_selectedControlPoint == TranslateSecond) {
        m_second = p;
    }
    else if (m_selectedControlPoint == TranslateThird) {
        m_third = p;
    }
    else if (m_selectedControlPoint == TranslateForth) {
        m_forth = p;

        if (m_inCreationMode) {
            QPoint diff = m_forth - m_first;
            m_second = m_first + (diff * 0.25);
            m_third  = m_first + (diff * 0.75);
        }
    }
    else if (m_selectedControlPoint == TranslateAll) {
        QPoint delta = p - m_translateReferencePoint;
        m_first  += delta;
        m_second += delta;
        m_third  += delta;
        m_forth  += delta;
        m_translateReferencePoint = p;
    }
}

void CurveEntity::deselectControlPoint() {
    m_selectedControlPoint = None;
    if (m_inCreationMode)
        m_inCreationMode = false;
}

void CurveEntity::setOutlined(bool o) {
    m_outlined = o;
    m_expectedTime = 0;
}

void CurveEntity::startOutlining(int expected) {
    m_outlineStartTime.start();
    m_expectedTime = expected;
}

void CurveEntity::paintEntity(QPainter& p) const {

    // TODO: redo this method for progress indicator
    p.setPen(QPen(Qt::green, 2,
        Qt::PenStyle(Qt::SolidLine),
        Qt::PenCapStyle(Qt::FlatCap),
        Qt::PenJoinStyle(Qt::MiterJoin)));

    QPainterPath path;
    path.moveTo(m_first);
    path.cubicTo(m_second, m_third, m_forth);

    p.drawPath(path);


    if (m_selected) {
        p.setPen(QPen(Qt::gray, 2,
            Qt::PenStyle(Qt::DotLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));

        p.drawLine(m_first, m_second);
        p.drawLine(m_third, m_forth);

        p.setPen(QPen(Qt::green, 2,
            Qt::PenStyle(Qt::SolidLine),
            Qt::PenCapStyle(Qt::FlatCap),
            Qt::PenJoinStyle(Qt::MiterJoin)));
        p.setBrush(Qt::white);

        p.drawEllipse(m_first, 5, 5);
        p.drawEllipse(m_second, 5, 5);
        p.drawEllipse(m_third, 5, 5);
        p.drawEllipse(m_forth, 5, 5);

        p.setBrush(Qt::NoBrush);
    }
}

QPoint CurveEntity::getStartPoint() const {
    return m_first;
}

std::list<std::list<QPoint> > CurveEntity::getListOfCurves() const {
    std::list<std::list<QPoint> > curves;

    std::list<QPoint> c;
    c.push_back(m_first);
    c.push_back(m_second);
    c.push_back(m_third);
    c.push_back(m_forth);
    curves.push_back(c);

    return curves;
}

void CurveEntity::setInCreationMode(bool b) {
    m_inCreationMode = b;
}

double CurveEntity::calcDistance(QPoint a, QPoint b) const {
    return qSqrt(qPow(a.x() - b.x(), 2) + qPow(a.y() - b.y(), 2));
}

double CurveEntity::estimateArcLengthOfCurve() const {
    double a = m_forth.x() - 3 * m_third.x() + 3 * m_second.x() - m_first.x();
    double b = 3 * m_third.x() - 6 * m_second.x() + 3 * m_first.x();
    double c = 3 * m_second.x() - 3 * m_first.x();
    double d = m_first.x();
    double e = m_forth.y() - 3 * m_third.y() + 3 * m_second.y() - m_first.y();
    double f = 3 * m_third.y() - 6 * m_second.y() + 3 * m_first.y();
    double g = 3 * m_second.y() - 3 * m_first.y();
    double h = m_first.y();

    QPoint prev = m_first;
    QPoint curr = m_first;
    double t;
    int x,y;
    double length = 0;;
    for (int i=0; i<26; i++) {
        prev = curr;
        t = (double) i / 25;
        x = qFloor( (((a*t) + b)*t + c)*t + d + 0.5 );
        y = qFloor( (((e*t) + f)*t + g)*t + h + 0.5 );
        curr = QPoint(x,y);
        length += calcDistance(prev, curr);
    }
    return length;
}

CurveEntity::ControlPoint CurveEntity::findControlPoint(QPoint p) const {
    if (calcDistance(p, m_forth) < 5.0)
        return TranslateForth;
    if (calcDistance(p, m_first) < 5.0)
        return TranslateFirst;
    if (calcDistance(p, m_second) < 5.0)
        return TranslateSecond;
    if (calcDistance(p, m_third) < 5.0)
        return TranslateThird;

    // detect for TranslateAll
    double a = m_forth.x() - 3 * m_third.x() + 3 * m_second.x() - m_first.x();
    double b = 3 * m_third.x() - 6 * m_second.x() + 3 * m_first.x();
    double c = 3 * m_second.x() - 3 * m_first.x();
    double d = m_first.x();
    double e = m_forth.y() - 3 * m_third.y() + 3 * m_second.y() - m_first.y();
    double f = 3 * m_third.y() - 6 * m_second.y() + 3 * m_first.y();
    double g = 3 * m_second.y() - 3 * m_first.y();
    double h = m_first.y();

    double t;
    int x,y;
    int iterations = qFloor( estimateArcLengthOfCurve() / 3 + 0.5 );
    for (int i=0; i<iterations; i++) {
        t = (double) i / iterations;
        x = qFloor( (((a*t) + b)*t + c)*t + d + 0.5 );
        y = qFloor( (((e*t) + f)*t + g)*t + h + 0.5 );
        if (calcDistance(QPoint(x,y), p) < 5.0) {
            return TranslateAll;
        }
    }

    return None;
}
