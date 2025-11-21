#include "server.h"

Server::Server() {
    bool ok;
    quint16 port = QInputDialog::getInt(nullptr, "Enter port", "Server stated on port:", 50000, 1, 65535, 1, &ok);
    if (!ok) {
        qDebug() << "server not started";
        exit(0);
    }
    if (this->listen(QHostAddress::Any,port))
        qDebug() << "server started on port" << port;
    else
        qDebug() << "error in constructor";
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
        sendClientsList();
    });

    socketList.append(socket);
    qDebug() << "new connection / size:" << socketList.size();
    sendClientsList();
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
    else qDebug() << "error in slotReadyRead";
}

void Server::sendClientsList() {
    QList<QHostAddress> clientsList;
    for (auto&it:socketList)
        clientsList.append(it->peerAddress());
    Datagram* datagram = new Datagram;
    datagram->Set_type(1);
    datagram->Set_list(clientsList);

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << *datagram;
    qDebug() << "sending clients list for" << socketList.size() << "clients";
    for (auto& it: socketList)
        it->write(data);
    delete datagram;
}
