#include <QObject>
#include <QTcpSocket>
#include <QBuffer>
#include <QDataStream>
#include <QMediaPlayer>
#include <QFile>

#include "socket.hpp"
#include "customvideosurface.hpp"

#pragma once

class Scope : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString socketState READ socketState NOTIFY socketStateChanged)
    Q_PROPERTY(QBuffer *buffer READ buffer NOTIFY bufferChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool isSingle READ isSingle NOTIFY isSingleChanged)
    Q_PROPERTY(bool isAuto READ isAuto NOTIFY isAutoChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY socketStateChanged)
    Q_PROPERTY(QMediaPlayer *mediaPlayer READ mediaPlayer NOTIFY mediaPlayerChanged)
    Q_PROPERTY(QAbstractVideoSurface *surface READ surface WRITE setSurface NOTIFY surfaceChanged)
public:
    explicit Scope(QObject *parent = nullptr);

    Q_INVOKABLE void connectToScope(QString address);
    Q_INVOKABLE void disconnectFromScope();

    Q_INVOKABLE void mouseEvent(quint32 x, quint32 y, bool pressed);
    Q_INVOKABLE void homeButtonEvent(bool pressed);
    Q_INVOKABLE void runButtonEvent(bool pressed);
    Q_INVOKABLE void singleButtonEvent(bool pressed);
    Q_INVOKABLE void autoButtonEvent(bool pressed);
    Q_INVOKABLE void halfButtonEvent(bool pressed);

    Q_INVOKABLE void quit();

    QString socketState();
    QBuffer *buffer();
    bool isRunning();
    bool isSingle();
    bool isAuto();
    bool isConnected();
    QMediaPlayer *mediaPlayer();
    QAbstractVideoSurface *surface();
    void setSurface(QAbstractVideoSurface *);

signals:
    void socketStateChanged();
    void bufferChanged();
    void isRunningChanged(bool isRunning);
    void isSingleChanged(bool isSingle);
    void isAutoChanged(bool isAuto);
    void mediaPlayerChanged();
    void surfaceChanged();

private:
    Socket *_socket = nullptr;
    QTcpSocket::SocketState _socketState = QTcpSocket::UnconnectedState;
    QMediaPlayer *_player = nullptr;
    QBuffer *_buffer = nullptr;
    QFile *_file = nullptr;
    std::shared_ptr<QVideoFrame> _currentFrame;

    QDataStream *_stream = nullptr;
    QAbstractVideoSurface *_surface = nullptr;

    void commonButtonEvent(uint8_t button, bool pressed);

    bool _isRunning = false;
    bool _isSingle = false;
    bool _isAuto = false;
};
