#ifndef CUSTOMVIDEOSURFACE_HPP
#define CUSTOMVIDEOSURFACE_HPP

#include <QObject>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

class CustomVideoSurface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface NOTIFY videoSurfaceChanged)
public:
    explicit CustomVideoSurface(QObject *parent = nullptr);

    QAbstractVideoSurface *videoSurface();
    void setVideoSurface(QAbstractVideoSurface *);


signals:
    void videoSurfaceChanged(QAbstractVideoSurface *);

private:
    QAbstractVideoSurface *_videoSurface = nullptr;
    QVideoSurfaceFormat _format;
};

#endif // CUSTOMVIDEOSURFACE_HPP
