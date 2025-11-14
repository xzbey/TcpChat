#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QObject>
#include <QCloseEvent>


class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server();

private:
    QByteArray data;
    QList<QTcpSocket*> socketList;

    void sendToClient(QString buffer);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
