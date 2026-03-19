#include "server.h"

Server::Server(QObject* parent) : QTcpServer(parent) {
    if (this->listen(QHostAddress::Any, 2323)) {
        qDebug() << "Server started on port 2323";
    } else {
        qDebug() << "Server error:" << this->errorString();
    }
}

void Server::incomingConnection(qintptr socketDescription) {
    auto* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescription);

    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::disconnected, [this, socket]() {
        Sockets.removeOne(socket);  // Удаляем из списка при отключении
    });

    Sockets.push_back(socket);
    qDebug() << "Client connected:" << socketDescription;
}

void Server::slotReadyRead(){
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);

    if (in.status() == QDataStream::Ok) {
        qDebug() << "Reading data...";
        QString str;
        in >> str;
        qDebug() << "Received:" << str;

        // Эхо-ответ тому же клиенту
        SendToClient(socket, "Server received: " + str);
    } else {
        qDebug() << "DataStream error:" << in.status();
    }
}

void Server::SendToClient(QTcpSocket* socket, const QString& str){
    QByteArray Data;
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << str;
    socket->write(Data);
}
