#ifndef STREAM_HPP
#define STREAM_HPP

#include <QObject>
#include <QTcpSocket>
#include <QBuffer>
#include <QDataStream>
#include <QMediaPlayer>
#include <QFile>

#include "socket.hpp"

class Stream : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString socketState READ socketState NOTIFY socketStateChanged)
    Q_PROPERTY(QBuffer *buffer READ buffer NOTIFY bufferChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool isSingle READ isSingle NOTIFY isSingleChanged)
    Q_PROPERTY(bool isAuto READ isAuto NOTIFY isAutoChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY socketStateChanged)
    Q_PROPERTY(QMediaPlayer *mediaPlayer READ mediaPlayer NOTIFY mediaPlayerChanged)
public:
    explicit Stream(QObject *parent = nullptr);

    Q_INVOKABLE void connectToScope(QString address);
    Q_INVOKABLE void disconnectFromScope();

    QString socketState();
    QBuffer *buffer();
    bool isRunning();
    bool isSingle();
    bool isAuto();
    bool isConnected();
    QMediaPlayer *mediaPlayer();

signals:
    void socketStateChanged();
    void bufferChanged();
    void isRunningChanged(bool isRunning);
    void isSingleChanged(bool isSingle);
    void isAutoChanged(bool isAuto);
    void mediaPlayerChanged();

private:
    Socket *_socket;
    QTcpSocket::SocketState _socketState = QTcpSocket::UnconnectedState;
    QMediaPlayer *_player;
    QBuffer *_buffer;
    QFile *_file;

    QDataStream *_stream;

    bool _isRunning = false;
    bool _isSingle = false;
    bool _isAuto = false;
};

#endif // STREAM_HPP
