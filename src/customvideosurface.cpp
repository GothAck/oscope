#include <QDebug>

#include "customvideosurface.hpp"

CustomVideoSurface::CustomVideoSurface(QObject *parent) :
    QObject(parent),
    _format(QSize {800, 600}, QVideoFrame::Format_YUV420P, QAbstractVideoBuffer::NoHandle)
    {}


QAbstractVideoSurface *CustomVideoSurface::videoSurface() {
    return _videoSurface;
}

void CustomVideoSurface::setVideoSurface(QAbstractVideoSurface *videoSurface) {
    qDebug() << __func__ << videoSurface;
    auto old = _videoSurface;
    _videoSurface = videoSurface;
    qDebug() << videoSurface->start(_format);
    old->deleteLater();
    emit videoSurfaceChanged(_videoSurface);
}
