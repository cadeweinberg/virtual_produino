#include "arduino_socket.h"

ArduinoSocket::ArduinoSocket(QObject *parent)
    : QUdpSocket(parent)
    , m_address("192.168.0.101")
    , m_port(50506)
{
    bind(m_address, m_port);
}

qint64 ArduinoSocket::writeDatagram(const QByteArray &bytes)
{
    return Parent::writeDatagram(bytes, m_address, m_port);
}
