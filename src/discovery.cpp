#include <QDebug>
#include <QNetworkDatagram>

#include "discovery.hpp"

DiscoveredNode::DiscoveredNode(QString address, QString name, QObject *parent) :
    QObject(parent),
    address(address),
    name(name)
    {}

Discovery::Discovery(QObject *parent) :
    QThread(parent),
    _socket(new QUdpSocket(this))
    {
        _socket->bind(12953);
        connect(_socket, &QUdpSocket::readyRead, [this]() {
            while (_socket->hasPendingDatagrams()) {
                QNetworkDatagram datagram = _socket->receiveDatagram();
                if (!datagram.destinationAddress().isEqual(QHostAddress("255.255.255.255"))) {
                    qDebug() << "not broadcast";
                    return;
                }
                auto data = datagram.data();
                if (!data.startsWith(QByteArray("\x22\x11\x55\xaa"))) {
                    qDebug() << "invalid magic";
                    return;
                }
                auto sender = datagram.senderAddress().toString();
                auto model = QString(data.mid(8, 10));
                auto serial = QString(data.mid(40, 10));
                auto id = model + " " + serial;

                if (_discovered.contains(sender)) {
                    if (_discovered[sender]->name == id) {
                        return;
                    }
                    _discovered[sender]->deleteLater();
                }
                _discovered[sender] = new DiscoveredNode(sender, id, this);
                emit discoveredChanged();
            }
        });
    }

QVariantList Discovery::discovered() {
    QVariantList list;
    for (auto v : _discovered.values()) {
        list.append(QVariant::fromValue(v));
    }
    return list;
}
