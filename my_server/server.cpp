#include "server.h"

Server::Server() {
    if (this->listen(QHostAddress::Any,50000)) {
        qDebug() << "server started";
    }
    else {
        qDebug() << "error in constructor";
    }
}

void Server::sendToClient(QString buffer) {
    data.clear();
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << buffer;
    //socket->write(data);

    for (auto& it: socketList) {
        it->write(data);
    }
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, [this, socket](){
        qDebug() << "one disconnect / size: " << socketList.size() - 1;
        socketList.removeOne(socket);
        socket->deleteLater();
    });

    socketList.append(socket);
    qDebug() << "new connection / size:" << socketList.size();
}

void Server::slotReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    QDataStream stream(socket);
    if (stream.status() == QDataStream::Ok) {
        QString buffer;
        stream >> buffer;
        qDebug() << "reading datastream /" << buffer;
        sendToClient(buffer);
    }
    else {
        qDebug() << "error in slotReadyRead";
    }

}
