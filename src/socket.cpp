#include <QtEndian>
#include <QDebug>

#include "socket.hpp"
#include "scope.hpp"

#define PARSE_SIZE 1024 * 1024 * 1

Socket::Socket(Scope *parent) :
    QThread(parent),
    _socket(new QTcpSocket(this)),
    _buffer(new QBuffer(this))
    {
        _buffer->open(QBuffer::WriteOnly);
        memset(&pic_raw, 0, sizeof(pic_raw));

        avcodec_register_all();
        pkt = av_packet_alloc();
        codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        parser = av_parser_init(codec->id);
        c = avcodec_alloc_context3(codec);
        picture = av_frame_alloc();
        sws = sws_getContext(800, 600, AV_PIX_FMT_YUV420P, 800, 600, AV_PIX_FMT_RGB32, 0, 0, 0, 0);

        picture->width = 800;
        picture->height = 600;
        if (avcodec_open2(c, codec, NULL) < 0) {
            return;
        }

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

        connect(this, &Socket::sendEvent, this, &Socket::onSendEvent);

        connect(parent, &Scope::isRunningChanged, [this](bool isRunning){
            _isRunning = isRunning;
        });
        connect(parent, &Scope::isSingleChanged, [this](bool isSingle){
            _isSingle = isSingle;
        });
        connect(parent, &Scope::isAutoChanged, [this](bool isAuto){
            _isAuto = isAuto;
        });
    }

QTcpSocket::SocketState Socket::socketState() {
    return _socket->state();
}

bool Socket::decode() {
    int ret;
    ret = avcodec_send_packet(c, pkt);
    if (ret < 0) {
        qDebug() << "avcodec_send_packet error" << ret;
        return true;
    }

    while(ret >= 0) {
        ret = avcodec_receive_frame(c, picture);
        switch (ret) {
        case AVERROR(EAGAIN):
            return true;
        case AVERROR_EOF:
            qDebug() << "avcodec_receive_frame EOF" << ret;
            return true;
        }
        if (ret < 0) {
            qDebug() << "avcodec_receive_frame error" << ret;
            return true;
        }

        auto bytes = (
            (picture->linesize[0] * 600) +
            (picture->linesize[1] * 600) +
            (picture->linesize[2] * 600)
        );

        auto vidFrame = std::make_shared<QVideoFrame>(800*600*4, QSize{800, 600}, 800 * 4, QVideoFrame::Format_RGB32);

        vidFrame->map(QAbstractVideoBuffer::WriteOnly);

        const int dstStride[1] = {800 * 4};
        uint8_t *bits[1] = {vidFrame->bits()};

        sws_scale(sws, picture->data, picture->linesize, 0, 600, bits, dstStride);

        vidFrame->unmap();
        emit frameAvailable(vidFrame);
    }
}

bool Socket::onFrame(QByteArray frame) {
    if (!frame.size()) return true;
    pkt->data = (uint8_t *)frame.data();
    pkt->size = frame.size();
    return decode();
}

void Socket::onSocketReadReady() {
    int bytes = _socket->bytesAvailable();
    while ((bytes >= 4) && isRunning()) {
        switch(_state) {
        case 0: {
            auto frameTypeArr = _socket->read(4);
            bytes -= 4;
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
                    emit setIsAuto(true);
                }
            } else if (_isAuto) {
                emit setIsAuto(false);
            }
            if ((frameType >> 2) & 1) {
                if (!_isSingle) {
                    emit setIsSingle(true);
                }
            } else if (_isSingle) {
                emit setIsSingle(false);
            }
            _state = 1;
            break;
        }
        case 1: {
            auto frameLenBytes = _socket->read(4);
            bytes -= 4;
            qFromLittleEndian<quint32>(frameLenBytes.data(), 4, &_frameLen);
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
                _state = 0;
                QByteArray buf = _buffer->buffer();
                if (onFrame(buf))
                    _buffer->reset();
                break;
            }
            QByteArray data = _socket->read(bytes < _frameLen ? bytes : _frameLen);
            _buffer->write(data);
            bytes -= data.size();
            _frameLen -= data.size();
            if (_frameLen <= 0) {
                _state = 0;
                QByteArray buf = _buffer->buffer();
                if (onFrame(buf))
                    _buffer->reset();
            }
            break;
        }
        }
    }
}

void Socket::onSendEvent(std::shared_ptr<QByteArray> event) {
    _socket->write(*event);
}
