#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <QObject>
#include <QThread>
#include <QTcpSocket>

class Stream;

class Socket : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QAbstractSocket::SocketState socketState READ socketState NOTIFY socketStateChanged)
public:
    Socket(Stream *parent);

    QAbstractSocket::SocketState socketState();

signals:
    void socketStateChanged(QTcpSocket::SocketState);
    void bytesAvailable(QByteArray array);
    void connectToHost(QString host);
    void disconnectFromHost();
    void setIsRunning(bool isRunning);
    void setIsSingle(bool isSingle);
    void setIsAuto(bool isAuto);

private slots:
    void onSocketReadReady();

private:
    Stream *_stream;
    QTcpSocket *_socket;
    int _state = 0;
    int _frameLen = 0;

    bool _isRunning = false;
    bool _isSingle = false;
    bool _isAuto = false;
};

#endif // SOCKET_HPP
