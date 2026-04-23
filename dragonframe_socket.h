#ifndef DRAGONFRAME_SOCKET_H
#define DRAGONFRAME_SOCKET_H

#include <QUdpSocket>

class DragonframeSocket : public QUdpSocket
{
    Q_OBJECT
public:
    DragonframeSocket(QObject *parent = nullptr);
};

#endif // DRAGONFRAME_SOCKET_H
