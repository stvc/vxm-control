#include "customvideosurface.h"

CustomVideoSurface::CustomVideoSurface(DrawableViewfinder* display, QObject* parent) :
    QAbstractVideoSurface(parent),
    m_widget(display),
    m_lastImage()
{
    this->m_mirror = false;
}

CustomVideoSurface::~CustomVideoSurface() {
}

QList<QVideoFrame::PixelFormat> CustomVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const {
    Q_UNUSED(handleType);

    return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB32;
}

bool CustomVideoSurface::present(const QVideoFrame &frame) {
    QVideoFrame f = frame;
    if (f.map(QAbstractVideoBuffer::ReadOnly)) {
        delete m_lastImage;
        QImage::Format format = QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat());
        m_lastImage = new QImage(frame.bits(), frame.width(), frame.height(), frame.bytesPerLine(), format);
        f.unmap();
        m_widget->setImage(m_lastImage->scaled(m_widget->width(), m_widget->height()).mirrored(this->m_mirror, false));
    }
    return true;
}

bool CustomVideoSurface::start(const QVideoSurfaceFormat &format) {
    return QAbstractVideoSurface::start(format);
}

void CustomVideoSurface::mirror(bool m) {
    this->m_mirror = m;
}
