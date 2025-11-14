#include "server.h"

Server::Server() {
    if (this->listen(QHostAddress::Any,50000)) {
        qDebug() << "server started";
    }
    else {
        qDebug() << "error in constructor";
    }
}

Server::~Server() {

}

void Server::sendToClient(Datagram* datagram) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << *datagram;
    //socket->write(data);
    qDebug() << "sending datagram for" << socketList.size() << "clients /" << datagram->Get_name() << datagram->Get_color().name() << datagram->Get_message();
    for (auto& it: socketList) {
        it->write(data);
    }
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, [this, socket](){
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
        Datagram* datagram = new Datagram;
        stream >> *datagram;
        qDebug() << "reading datagram /" << datagram->Get_name() << datagram->Get_color().name() << datagram->Get_message();
        sendToClient(datagram);
        delete datagram;
    }
    else {
        qDebug() << "error in slotReadyRead";
    }

}
