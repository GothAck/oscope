#include <memory>

#include <QtEndian>
#include <QDebug>
#include <QtConcurrent>

#include "stream.hpp"

Stream::Stream(QObject *parent) :
    QObject(parent),
    _socket(new Socket(this)),
    _player(new QMediaPlayer(this, QMediaPlayer::StreamPlayback)),
    _buffer(new QBuffer(this)),
    _file(new QFile("/tmp/oscope.dump", this)),
    _stream(new QDataStream(_file))
    {
        _file->open(QFile::WriteOnly);
        _buffer->open(QBuffer::ReadWrite);
        _socket->start();
        connect(_socket, &Socket::socketStateChanged, [this](QTcpSocket::SocketState state) {
            _socketState = state;
            emit socketStateChanged();
        });
        connect(_socket, &Socket::bytesAvailable, [this](std::shared_ptr<QByteArray> bytes) {
//            _buffer->write(bytes);
//            qDebug() << "Stream write" << bytes->size();
            _stream->writeRawData(bytes->data(), bytes->size());
//            _player->setMedia(QMediaContent(), _buffer);
//            _player->play();
        });
        connect(_socket, &Socket::setIsRunning, [this](bool isRunning) {
//            qDebug() << "setIsRunning" << isRunning;
            _isRunning = isRunning;
            isRunningChanged(_isRunning);
        });
        connect(_socket, &Socket::setIsSingle, [this](bool isSingle) {
//            qDebug() << "setIsSingle" << isSingle;
            _isSingle = isSingle;
            isSingleChanged(_isSingle);
        });
        connect(_socket, &Socket::setIsAuto, [this](bool isAuto) {
//            qDebug() << "setIsAuto" << isAuto;
            _isAuto = isAuto;
            isAutoChanged(_isAuto);
        });
        connect(this, &Stream::destroyed, [this] {
            _socket->quit();
            _socket->wait(1000);
        });
        connect(_socket, &Socket::frameAvailable, [this](std::shared_ptr<QVideoFrame> frame) {
            if (_surface)
                qDebug() << "present" << _surface->present(*frame);
            _currentFrame = frame;
        });
    }

void Stream::connectToScope(QString address) {
    _socket->connectToHost(address);
}

void Stream::disconnectFromScope() {
    _socket->disconnectFromHost();
}

QString Stream::socketState() {
    auto metaEnum = QMetaEnum::fromType<QTcpSocket::SocketState>();
    return QString(metaEnum.valueToKey(_socketState)).replace("State", "");
}

QBuffer *Stream::buffer() {
    return _buffer;
}

bool Stream::isRunning() {
    return _isRunning;
}

bool Stream::isSingle() {
    return _isSingle;
}

bool Stream::isAuto() {
    return _isAuto;
}

bool Stream::isConnected() {
    return _socketState != QTcpSocket::UnconnectedState;
}

QMediaPlayer *Stream::mediaPlayer() {
    return _player;
}

QAbstractVideoSurface *Stream::surface() {
    return _surface;
}

void Stream::setSurface(QAbstractVideoSurface *surface) {
    _surface = surface;
    emit surfaceChanged();
}

