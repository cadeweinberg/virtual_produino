#include "dragonframe_socket.h"

DragonframeSocket::DragonframeSocket(QObject *parent)
    : QUdpSocket(parent)
    , m_address(QHostAddress::LocalHost)
    , m_port(8888)
{
    bind(m_address, m_port);
}

qint64 DragonframeSocket::writeDatagram(const QByteArray &bytes)
{
    return Parent::writeDatagram(bytes, m_address, m_port);
}