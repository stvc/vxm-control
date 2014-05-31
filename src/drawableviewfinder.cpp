#include "drawableviewfinder.h"

DrawableViewfinder::DrawableViewfinder(QWidget *parent) :
    QWidget(parent)
{
    m_mode = Pointer;
    m_selectedEntity = NULL;

    m_crosshair_size = 15;
    m_crosshairs = QPoint(this->width() / 2, this->height() / 2);

    setAutoFillBackground(false);

    frame = new QImage(QSize(0,0), QImage::Format_ARGB32_Premultiplied);
    this->m_freezeFrame = false;

    setFocusPolicy(Qt::StrongFocus);

    // this is for changing the mouse cursor when mousing over control points
    // of currently selected entity (currently disabled)
//    setMouseTracking(true);
}

DrawableViewfinder::~DrawableViewfinder() {
    delete frame;
}

void DrawableViewfinder::setImage(QImage img) {
    if (!m_freezeFrame) {
        delete this->frame;
        this->frame = new QImage(img);
    }

    this->update();
}

void DrawableViewfinder::freezeFrame(bool b) {
    m_freezeFrame = b;
}

void DrawableViewfinder::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), *frame);
    painter.setPen(QPen(Qt::white, 2,
                Qt::PenStyle(Qt::SolidLine),
                Qt::PenCapStyle(Qt::FlatCap),
                Qt::PenJoinStyle(Qt::MiterJoin)));
    painter.drawLine(QPoint(m_crosshairs.x() - m_crosshair_size, m_crosshairs.y()),
            QPoint(m_crosshairs.x() + m_crosshair_size, m_crosshairs.y()));
    painter.drawLine(QPoint(m_crosshairs.x(), m_crosshairs.y() - m_crosshair_size),
            QPoint(m_crosshairs.x(), m_crosshairs.y() + m_crosshair_size));
    painter.setBackground(Qt::NoBrush);

    if (!m_entities.empty() && m_mode != CalibrationMode) {
        for (std::list<DrawableEntity*>::iterator it=m_entities.begin(); it != m_entities.end(); ++it) {
            DrawableEntity* tmp = *it;
            tmp->paintEntity(painter);
        }
    }
}

void DrawableViewfinder::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete && m_selectedEntity != NULL) {
        removeSelectedEntity();
    }
    else if (event->key() == Qt::Key_Escape && m_selectedEntity != NULL) {
        deselectEntity();
    }
    else if (event->key() == Qt::Key_Space && m_selectedEntity != NULL) {
        // TODO: only for testing progress indicator, remove for release
        m_selectedEntity->setOutlined(false);
        m_selectedEntity->startOutlining(10 * 1000);
    }
    else if (event->key() == Qt::Key_Shift && m_selectedEntity != NULL) {
        m_selectedEntity->setShiftKeyState(true);
    }
    else {
        QWidget::keyPressEvent(event);
    }
}

void DrawableViewfinder::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Shift && m_selectedEntity != NULL) {
        m_selectedEntity->setShiftKeyState(false);
    }
}
void DrawableViewfinder::mousePressEvent(QMouseEvent* event) {
    if (m_mode == Pointer) {
        if (m_selectedEntity != NULL) {
            if (!m_selectedEntity->isPointOnEntity(event->pos())) {
                deselectEntity();
            }
            else {
                m_selectedEntity->selectControlPoint(event->pos());
            }
        }
        else {
            m_clickPosition = event->pos();
        }
    }
    else if (m_mode == Line) {
        m_entities.push_back(new LineEntity(event->pos()));
        m_selectedEntity = m_entities.back();
        m_selectedEntity->setSelected(true);
        m_selectedEntity->selectControlPoint(event->pos());
    }
    else if (m_mode == Rectangle) {
        m_entities.push_back(new RectangleEntity(event->pos()));
        m_selectedEntity = m_entities.back();
        m_selectedEntity->setSelected(true);
        m_selectedEntity->selectControlPoint(event->pos());
    }
    else if (m_mode == Circle) {
        m_entities.push_back(new EllipseEntity(event->pos()));
        m_selectedEntity = m_entities.back();
        m_selectedEntity->setSelected(true);
        m_selectedEntity->selectControlPoint(event->pos());
    }
    else if (m_mode == Polygon) {
        m_entities.push_back(new PolygonEntity(event->pos()));
        m_selectedEntity = m_entities.back();
        m_selectedEntity->setSelected(true);
        m_selectedEntity->selectControlPoint(event->pos());
    }
    else if (m_mode == Curve) {
        m_entities.push_back(new CurveEntity(event->pos()));
        m_selectedEntity = m_entities.back();
        m_selectedEntity->setSelected(true);
        m_selectedEntity->selectControlPoint(event->pos());
    }

    update();
}

void DrawableViewfinder::mouseReleaseEvent(QMouseEvent* event) {
    if (m_mode == CalibrationMode) {
        m_clickPosition = event->pos();
        emit pointClicked();
    }
    else if (m_selectedEntity != NULL) {
        // reset m_selectedEntity's control point if necessary
        m_selectedEntity->deselectControlPoint();

        // just drew a new shape
        if (m_mode != Pointer) {
            emit entityAdded();
        }
        emit entityChanged();
    }
    else {
        if (event->pos() == m_clickPosition) {
            // user clicked in a single position without moving
            // check if point is on any entities, and if it is select that
            // entity and emit signal
            for (std::list<DrawableEntity*>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
                if ((*it)->isPointOnEntity(event->pos())) {
                    (*it)->setSelected(true);
                    m_selectedEntity = *it;
                    emit entityChanged();
                    break;
                }
            }
        }
    }

    update();
}

void DrawableViewfinder::mouseMoveEvent(QMouseEvent* event) {
    if (m_selectedEntity != NULL) {
//        if (event->button() == Qt::NoButton) {
//            setCursor(m_selectedEntity->getMouseCursorAtPosition(event->pos()));
//        }
//        else {
            m_selectedEntity->moveControlPointTo(event->pos());
            emit entityChanged();
//        }
    }

    update();
}

bool DrawableViewfinder::isEntitySelected() {
    return (m_selectedEntity != NULL);
}

void DrawableViewfinder::setEntity(Entity e) {
    m_mode = e;
}

DrawableViewfinder::Entity DrawableViewfinder::getEntity() {
    return m_mode;
}


void DrawableViewfinder::setCrosshairs(QPoint p) {
    m_crosshairs = p;
}

void DrawableViewfinder::addEntity(DrawableEntity* e) {
    m_entities.push_back(e);
}

void DrawableViewfinder::selectEntityAtPoint(QPoint p) {
    // deselect currently selected
    deselectEntity();

    // select
    for (std::list<DrawableEntity*>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
        DrawableEntity* tmp = *it;
        if (tmp->isPointOnEntity(p)) {
            tmp->setSelected(true);
            m_selectedEntity = tmp;
            break;
        }
    }
}

void DrawableViewfinder::deselectEntity() {
    if (m_selectedEntity != NULL) {
        m_selectedEntity->setSelected(false);
        m_selectedEntity->setShiftKeyState(false);
        m_selectedEntity = NULL;
    }
}

void DrawableViewfinder::removeSelectedEntity() {
    if (m_selectedEntity != NULL) {
        // TODO: possible memory leaks
        m_entities.remove(m_selectedEntity);
        m_selectedEntity = NULL;
    }
}

QPoint DrawableViewfinder::getClickPosition() {
    return m_clickPosition;
}

std::list<DrawableEntity*> *DrawableViewfinder::getListOfEntities() {
    return &m_entities;
}

