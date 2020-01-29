#include <QtEndian>
#include <QDebug>
#include <QtConcurrent>

#include "stream.hpp"

Stream::Stream(QObject *parent) :
    QObject(parent),
    _socket(new Socket(this)),
    _buffer(new QBuffer(this)),
    _stream(new QDataStream(_buffer)),
    _player(new QMediaPlayer(this, QMediaPlayer::StreamPlayback))
    {
        _buffer->open(QBuffer::ReadWrite);
        _socket->start();
        connect(_socket, &Socket::socketStateChanged, [this](QTcpSocket::SocketState state) {
            _socketState = state;
            emit socketStateChanged();
        });
        connect(_socket, &Socket::bytesAvailable, [this](QByteArray bytes) {
            _buffer->write(bytes);
            _player->setMedia(QMediaContent(), _buffer);
            _player->play();
        });
        connect(_socket, &Socket::setIsRunning, [this](bool isRunning) {
            qDebug() << "setIsRunning" << isRunning;
            _isRunning = isRunning;
            isRunningChanged(_isRunning);
        });
        connect(_socket, &Socket::setIsSingle, [this](bool isSingle) {
            qDebug() << "setIsSingle" << isSingle;
            _isSingle = isSingle;
            isSingleChanged(_isSingle);
        });
        connect(_socket, &Socket::setIsAuto, [this](bool isAuto) {
            qDebug() << "setIsAuto" << isAuto;
            _isAuto = isAuto;
            isAutoChanged(_isAuto);
        });
        connect(this, &Stream::destroyed, [this] {
            _socket->quit();
            _socket->wait(1000);
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

