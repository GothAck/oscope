#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <memory>

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QBuffer>
#include <QVideoFrame>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libswscale/swscale.h"
}

class Scope;

class Socket : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QAbstractSocket::SocketState socketState READ socketState NOTIFY socketStateChanged)
public:
    Socket(Scope *parent);

    QAbstractSocket::SocketState socketState();

    bool decode();
    bool onFrame(QByteArray frame);

signals:
    void socketStateChanged(QTcpSocket::SocketState);
    void bytesAvailable(std::shared_ptr<QByteArray> array);
    void frameAvailable(std::shared_ptr<QVideoFrame> frame);

    void connectToHost(QString host);
    void disconnectFromHost();

    void sendEvent(std::shared_ptr<QByteArray> event);

    void setIsRunning(bool isRunning);
    void setIsSingle(bool isSingle);
    void setIsAuto(bool isAuto);

private slots:
    void onSocketReadReady();
    void onSendEvent(std::shared_ptr<QByteArray> event);

private:
    // x264 stuff
    const AVCodec *codec;
    AVCodecParserContext *parser = nullptr;
    AVCodecContext *c= nullptr;
    AVFrame *picture = nullptr;
    AVPicture pic_raw;
    AVPacket *pkt = nullptr;

    int num_nals = 0;
    int pts;
    struct SwsContext* sws = nullptr;

    // everything else
    Scope *_stream;
    QTcpSocket *_socket;
    QBuffer *_buffer;
    int _state = 0;
    int _frameLen = 0;

    bool _isRunning = false;
    bool _isSingle = false;
    bool _isAuto = false;
};

#endif // SOCKET_HPP
