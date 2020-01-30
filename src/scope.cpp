#include <memory>

#include <QtEndian>
#include <QDebug>
#include <QtConcurrent>

#include "scope.hpp"

Scope::Scope(QObject *parent) :
    QObject(parent),
    _player(new QMediaPlayer(this, QMediaPlayer::StreamPlayback)),
    _buffer(new QBuffer(this)),
    _file(new QFile("/tmp/oscope.dump", this)),
    _stream(new QDataStream(_file))
    {
        _file->open(QFile::WriteOnly);
        _buffer->open(QBuffer::ReadWrite);
    }

void Scope::connectToScope(QString address) {
    _socket = new Socket(this);

    connect(_socket, &Socket::socketStateChanged, [this](QTcpSocket::SocketState state) {
        _socketState = state;
        emit socketStateChanged();
    });
    connect(_socket, &Socket::bytesAvailable, [this](std::shared_ptr<QByteArray> bytes) {
        _stream->writeRawData(bytes->data(), bytes->size());
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
    connect(_socket, &Socket::frameAvailable, [this](std::shared_ptr<QVideoFrame> frame) {
        if (_surface)
            _surface->present(*frame);
        _currentFrame = frame;
    });

    _socket->connectToHost(address);
}

void Scope::disconnectFromScope() {
    if (!_socket)
        return;
    _socket->disconnectFromHost();
    _socket->quit();
    _socket->wait(1000);
    _socket->terminate();
    _socket->wait(1000);
    _socket->deleteLater();
    _socket = nullptr;
}

void Scope::mouseEvent(quint32 x, quint32 y, bool pressed) {
    if (!_socket)
        return;
    auto b = std::make_shared<QByteArray>();
    QByteArray num(4, 0);
    b->append(1);
    b->append(!!pressed);
    b->append((uint8_t)0);
    b->append((uint8_t)0);
    qToLittleEndian<quint32>(&x, 4, num.data());
    b->append(num);
    qToLittleEndian<quint32>(&y, 4, num.data());
    b->append(num);
    emit _socket->sendEvent(b);
}

void Scope::homeButtonEvent(bool pressed) {
    commonButtonEvent(97, pressed);
}
void Scope::runButtonEvent(bool pressed) {
    commonButtonEvent(8, pressed);
}
void Scope::singleButtonEvent(bool pressed) {
    commonButtonEvent(9, pressed);
}
void Scope::autoButtonEvent(bool pressed) {
    commonButtonEvent(48, pressed);
}
void Scope::halfButtonEvent(bool pressed) {
    commonButtonEvent(49, pressed);
}
void Scope::commonButtonEvent(uint8_t button, bool pressed) {
    if (!_socket)
        return;
    auto b = std::make_shared<QByteArray>(
        "\x02\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01",
        12);
    b->data()[4] = button;
    b->data()[11] = pressed;
    emit _socket->sendEvent(b);
}

void Scope::quit() {
    if (!_socket)
        return;
    _socket->disconnectFromHost();
    _socket->quit();
    _socket->wait(1000);
}

QString Scope::socketState() {
    auto metaEnum = QMetaEnum::fromType<QTcpSocket::SocketState>();
    return QString(metaEnum.valueToKey(_socketState)).replace("State", "");
}

QBuffer *Scope::buffer() {
    return _buffer;
}

bool Scope::isRunning() {
    return _isRunning;
}

bool Scope::isSingle() {
    return _isSingle;
}

bool Scope::isAuto() {
    return _isAuto;
}

bool Scope::isConnected() {
    return _socketState != QTcpSocket::UnconnectedState;
}

QMediaPlayer *Scope::mediaPlayer() {
    return _player;
}

QAbstractVideoSurface *Scope::surface() {
    return _surface;
}

void Scope::setSurface(QAbstractVideoSurface *surface) {
    _surface = surface;
    emit surfaceChanged();
}

