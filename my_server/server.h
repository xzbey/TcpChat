#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QObject>
#include <QCloseEvent>
#include <QApplication>
#include <QInputDialog>

#include "datagram.h"


class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server();

private:
    QList<QTcpSocket*> socketList;
    quint64 nextBlockSize = 0;

    void sendToClient(Datagram* datagram);
    void sendClientsList();

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
