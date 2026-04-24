#ifndef DRAGONFRAME_SOCKET_H
#define DRAGONFRAME_SOCKET_H

#include <QUdpSocket>

class DragonframeSocket : public QUdpSocket
{
    Q_OBJECT
public:
    using Parent = QUdpSocket;

    DragonframeSocket(QObject *parent = nullptr);

    QHostAddress address() const { return m_address; }
    void address(QHostAddress a) { m_address = a; }
    quint16 port() const { return m_port; }
    void port(quint16 p) { m_port = p; }

    qint64 writeDatagram(const QByteArray &bytes);

private:
    QHostAddress m_address;
    quint16      m_port;
};

#endif // DRAGONFRAME_SOCKET_H
