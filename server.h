#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDebug>
#include <QDataStream>
#include <QHash>

class Server : public QTcpServer {
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);

private:
    QVector<QTcpSocket*> Sockets;
    QHash<QTcpSocket*, quint32> expectedSizes;
    void SendToClient(QTcpSocket* socket, const QString& str);

public slots:
    void incomingConnection(qintptr socketDescription) override;
    void slotReadyRead();
};

#endif // SERVER_H
