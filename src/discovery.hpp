#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include <QString>
#include <QMap>
#include <QPair>

#pragma once

class DiscoveredNode : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString address MEMBER address CONSTANT)
    Q_PROPERTY(QString name MEMBER name CONSTANT)
public:
    DiscoveredNode(QString address, QString name, QObject *parent);

    QString const address;
    QString const name;
};

class Discovery : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QVariantList discovered READ discovered NOTIFY discoveredChanged)
public:
    Discovery(QObject *parent);

    QVariantList discovered();

signals:
    void discoveredChanged();

private:
    QMap<QString, DiscoveredNode*> _discovered;
    QUdpSocket *_socket;
};
