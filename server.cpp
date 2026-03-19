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
        Sockets.removeOne(socket);
        expectedSizes.remove(socket);
    });

    Sockets.push_back(socket);
    expectedSizes[socket] = 0;
    qDebug() << "Client connected:" << socketDescription;
}

void Server::slotReadyRead() {
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QDataStream in(socket);

    if (expectedSizes[socket] == 0) {
        if (socket->bytesAvailable() < static_cast<int>(sizeof(quint32))) {
            return;
        }
        quint32 size = 0;
        in >> size;
        expectedSizes[socket] = size;
    }

    if (socket->bytesAvailable() < expectedSizes[socket]) {
        return;
    }

    // Читаем сообщение целиком
    QByteArray data = socket->read(expectedSizes[socket]);
    QString str = QString::fromUtf8(data);
    expectedSizes[socket] = 0;

    qDebug() << "Received:" << str;

    SendToClient(socket, "Server received: " + str);
}

void Server::SendToClient(QTcpSocket* socket, const QString& str) {
    QByteArray payload = str.toUtf8();
    QByteArray packet;

    QDataStream out(&packet, QIODevice::WriteOnly);
    out << static_cast<quint32>(payload.size());
    packet.append(payload);

    socket->write(packet);
}
