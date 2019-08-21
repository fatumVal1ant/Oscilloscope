#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "server.h"
#include "tcpclient.h"

namespace oscilloscope {
    class TcpServer : public Server {
        Q_OBJECT

    private:
        QTcpServer *_server;

    public:
        explicit TcpServer(quint16 port, QObject *parent = nullptr);

        bool start();
        void stop();

        ~TcpServer();

    private slots:
        void receiveConnection();
        void receiveData(TcpClient *client);

    };
}

#endif // TCPSERVER_H
