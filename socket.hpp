#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <memory>

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QBuffer>
#include <QVideoFrame>

#include <wels/codec_api.h>

class Stream;

class Socket : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QAbstractSocket::SocketState socketState READ socketState NOTIFY socketStateChanged)
public:
    Socket(Stream *parent);

    QAbstractSocket::SocketState socketState();

    bool onFrame(QByteArray frame);

signals:
    void socketStateChanged(QTcpSocket::SocketState);
    void bytesAvailable(std::shared_ptr<QByteArray> array);
    void frameAvailable(std::shared_ptr<QVideoFrame> frame);
    void connectToHost(QString host);
    void disconnectFromHost();
    void setIsRunning(bool isRunning);
    void setIsSingle(bool isSingle);
    void setIsAuto(bool isAuto);

private slots:
    void onSocketReadReady();

private:
    // openh264 stuff
    ISVCDecoder *_pSvcDecoder;
    SBufferInfo  _sDstBufInfo;
    SParserBsInfo _sDstParseInfo;
    SDecodingParam _sDecParam = {0};
    unsigned char *_pData[4];

    // everything else
    Stream *_stream;
    QTcpSocket *_socket;
    QBuffer *_buffer;
    int _state = 0;
    int _frameLen = 0;

    bool _isRunning = false;
    bool _isSingle = false;
    bool _isAuto = false;
};

#endif // SOCKET_HPP
