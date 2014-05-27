#ifndef DRAWABLEENTITY_H
#define DRAWABLEENTITY_H

#include <list>
#include <QPainter>
#include <QPoint>
#include <QTime>

/**
 * It is assumed that each entity is continuous, so the method
 * getListOfCurves() should return a list of lists of control points, with the
 * first point of the first list matching the point returned with
 * getStartPoint(), and the last point in each list matching the first point in
 * the next list. The points should be absolute points and will be converted
 * from pixel-space to step-space elsewhere. A straight line is expressed as a
 * list of two points, and a curve is expressed as a list of 4 points (cubic
 * bezier curve). Lists of any other size will cause undefined behavior.
 *
 * The methods selectControlPoint, moveControlPointTo, and deselectControlPoint
 * are called everytime the entity is selected and a user clicks the mouse,
 * drags, and releases (respectively). If the user clicks somewhere where the
 * isPointOnEntity method returns false, these methods are not called. Instead,
 * setSelected(false) is called.
 *
 * Some entities may require interaction with the Shift key (e.g. a line
 * conforming to 90 degrees when the shift key is pressed or an ellipse
 * conforming to a circle when the shift key is pressed). The status of the
 * shift key can be determined with the m_shiftKeyState member variable, which
 * is set accordingly whenever the entity is selected and the shift key is
 * toggled.
 *
 * The setOutlined method is called to tell the entity to draw itself as
 * outlined or not. It is usually called with a parameter of false when the
 * entity is queued for drawing, and with a parameter of true when drawing the
 * entity is complete. The startOutlining method is called when the controller
 * starts drawing the shape, and takes an integer indicating the number of
 * milliseconds it is expected to take. Each entity should calculate (based on
 * how much time has passed since calling the startOutlining method) the
 * percentage of the shape that is drawn, and draw itself accordingly.
 */

class DrawableEntity
{
public:
    DrawableEntity() : CONTROL_POINT_RADIUS(2) { m_selected = false; m_shiftKeyState = false; };
    virtual ~DrawableEntity() {};

    virtual void setSelected(bool s) { m_selected = s; };
    virtual bool isSelected() const { return m_selected; };
    virtual void setShiftKeyState(bool s) { m_shiftKeyState = s; };
    virtual bool isPointOnEntity(QPoint) const = 0;

    virtual Qt::CursorShape getMouseCursorAtPosition(QPoint) const = 0;
    virtual void selectControlPoint(QPoint) = 0;
    virtual void moveControlPointTo(QPoint) = 0;
    virtual void deselectControlPoint() = 0;

    virtual void setOutlined(bool) = 0;
    virtual void startOutlining(int) = 0;
    virtual void paintEntity(QPainter&) const = 0;

    virtual QPoint getStartPoint() const = 0;

    virtual std::list<std::list<QPoint> > getListOfCurves() const = 0;

protected:
    int const CONTROL_POINT_RADIUS;
    bool m_selected;
    bool m_shiftKeyState;
};


#endif
