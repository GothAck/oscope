#include <QtEndian>
#include <QDebug>

#include "socket.hpp"
#include "stream.hpp"

#define PARSE_SIZE 1024 * 1024 * 1

Socket::Socket(Stream *parent) :
    QThread(parent),
    _socket(new QTcpSocket(this)),
    _buffer(new QBuffer(this))
    {
        _buffer->open(QBuffer::WriteOnly);
        memset(&_sDstBufInfo, 0, sizeof(SBufferInfo));
        _sDstBufInfo.UsrData.sSystemBuffer.iWidth = 800;
        _sDstBufInfo.UsrData.sSystemBuffer.iHeight = 600;
        _sDstBufInfo.UsrData.sSystemBuffer.iFormat = videoFormatI420;
        memset(&_sDstParseInfo, 0, sizeof(SParserBsInfo));
        _sDstParseInfo.pDstBuff = new unsigned char[PARSE_SIZE]; //In Parsing only, allocate enough buffer to save transcoded bitstream for a frame
        _pData[0] = new unsigned char [PARSE_SIZE];
        _pData[1] = new unsigned char [PARSE_SIZE];
        _pData[2] = new unsigned char [PARSE_SIZE];

        qDebug() << "pdata" << _pData[0] << _pData[1] << _pData[2];

        WelsCreateDecoder(&_pSvcDecoder);

        _sDecParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_AVC;
        _sDecParam.bParseOnly = false;

        _pSvcDecoder->Initialize(&_sDecParam);

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

bool Socket::onFrame(QByteArray frame) {
    if (!frame.size()) return true;
//    if (frame.size() == 4) return true;
    qDebug() << __func__ << frame.size();
    int ret;

//    ret = _pSvcDecoder->DecodeParser((const unsigned char *)frame.constData(), frame.size(), &_sDstParseInfo);
    ret = _pSvcDecoder->DecodeFrameNoDelay(
        (const unsigned char *)frame.constData(),
        frame.size(),
        _pData,
        &_sDstBufInfo);

    if (ret) {
        qDebug() << "Error occurred" << ret;
        return true;
    } else {
        if (!_sDstBufInfo.iBufferStatus) {
            return false;
        }
        qDebug() << "Parse data" << _sDstParseInfo.iNalNum << _sDstBufInfo.iBufferStatus << _sDstParseInfo.pNalLenInByte << _sDstBufInfo.UsrData.sSystemBuffer.iStride;
        auto vidFrame = std::make_shared<QVideoFrame>(3 * 800 * 600, QSize{800, 600}, 800, QVideoFrame::Format_YV12);
        vidFrame->map(QAbstractVideoBuffer::WriteOnly);

        for (int i = 0; i < 3; i++) {
            qDebug() << "bits" << i << vidFrame->bits(i) << _pData[i] << vidFrame->planeCount() << vidFrame->bytesPerLine(i) * 600;
            if (!_pData[i]) continue;
            memcpy(vidFrame->bits(i), _pData[i] , 800* 600);
        }
//        for (int x = 0; x < (800 * 600); x++) {
//            qDebug() << *(&_pData[0] + x);
//        }
        vidFrame->unmap();
        emit frameAvailable(vidFrame);
        return true;
    }
    return true;

    // Old
    auto shared = std::make_shared<QByteArray>(frame.constData(), frame.size());
    emit bytesAvailable(shared);
}

void Socket::onSocketReadReady() {
//    qDebug() << __func__ << _socket->bytesAvailable();
    int bytes = _socket->bytesAvailable();
    while ((bytes >= 4) && isRunning()) {
        switch(_state) {
        case 0: {
            auto frameTypeArr = _socket->read(4);
            bytes -= 4;
//            qDebug() << frameTypeArr;
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
//                qDebug() << "0 bytes" << _frameLen;
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
