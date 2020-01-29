#include <QtEndian>
#include <QDebug>

#include "socket.hpp"
#include "stream.hpp"

Socket::Socket(Stream *parent) :
    QThread(parent),
    _socket(new QTcpSocket(this))
    {
        connect(_socket, &QTcpSocket::stateChanged, this, &Socket::socketStateChanged);
        connect(_socket, &QTcpSocket::readyRead, this, &Socket::onSocketReadReady);
        connect(this, &Socket::connectToHost, [this](QString host) {
            qDebug() << "connectToHost" << host;
           _socket->connectToHost(host, 8888) ;
        });
        connect(this, &Socket::disconnectFromHost, [this]{
            qDebug() << "disconnectFromHost";
           _socket->disconnectFromHost();
        });
        connect(parent, &Stream::isRunningChanged, [this](bool isRunning){
            _isRunning = isRunning;
        });
        connect(parent, &Stream::isSingleChanged, [this](bool isSingle){
            _isSingle = isSingle;
        });
        connect(parent, &Stream::isAutoChanged, [this](bool isAuto){
            _isAuto = isAuto;
        });
    }

QTcpSocket::SocketState Socket::socketState() {
    return _socket->state();
}

void Socket::onSocketReadReady() {
    qDebug() << __func__ << _socket->bytesAvailable();
    int bytes = _socket->bytesAvailable();
    while ((bytes >= 4) && isRunning()) {
        switch(_state) {
        case 0: {
            auto frameTypeArr = _socket->read(4);
            bytes -= 4;
            qDebug() << frameTypeArr;
            auto frameType = frameTypeArr[1];
            if ((frameType >> 7) & 1) {
                if (_isRunning) {
                    emit setIsRunning(false);
                }
            } else if (!_isRunning) {
                emit setIsRunning(true);
            }
            if ((frameType >> 5) & 1) {
                if (!_isAuto) {
                    emit setIsAuto(false);
                }
            } else if (_isAuto) {
                emit setIsAuto(true);
            }
            if ((frameType >> 2) & 1) {
                if (!_isSingle) {
                    emit setIsSingle(false);
                }
            } else if (_isSingle) {
                emit setIsSingle(true);
            }
            _state = 1;
            break;
        }
        case 1: {
            auto frameLenBytes = _socket->read(4);
            bytes -= 4;
            qFromBigEndian<quint32>(frameLenBytes.data(), 4, &_frameLen);
            if (_frameLen < 1048576) {
                _state = 2;
            } else {
                qDebug() << "_frameLen too large?!";
                _state = 0;
                _frameLen = 0;
            }
            break;;
        }
        case 2: {
            if(_frameLen <= 0) {
                qDebug() << "0 bytes" << _frameLen;
                _state = 0;
                break;
            }
            auto data = _socket->read(bytes < _frameLen ? bytes : _frameLen);
            emit bytesAvailable(data);
            _frameLen -= data.size();
            if (_frameLen <= 0) {
                _state = 0;
            }
            break;
        }
        }
    }
}
