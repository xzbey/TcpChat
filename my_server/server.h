#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QObject>
#include <QCloseEvent>
#include "datagram.h"


class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server();

private:
    QList<QTcpSocket*> socketList;

    void sendToClient(Datagram* datagram);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
