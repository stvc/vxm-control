#ifndef CUSTOMVIDEOSURFACE_H
#define CUSTOMVIDEOSURFACE_H

#include "drawableviewfinder.h"
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

class CustomVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    explicit CustomVideoSurface(DrawableViewfinder* display, QObject* parent = 0);
    virtual ~CustomVideoSurface();

    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    virtual bool present(const QVideoFrame &frame);
    virtual bool start(const QVideoSurfaceFormat &format);
    void mirror(bool);

private:
    DrawableViewfinder* m_widget;
    QImage* m_lastImage;
    bool m_mirror;
};

#endif
