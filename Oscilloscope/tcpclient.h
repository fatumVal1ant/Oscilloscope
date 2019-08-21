#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

#include "frame.h"

namespace oscilloscope {
    class TcpClient : public QObject {
        Q_OBJECT

    private:
        QTcpSocket *_socket;

    public:
        explicit TcpClient(QTcpSocket* client, QObject* parent = nullptr);

        Frame *read();

        ~TcpClient();

    signals:
        void readyRead(TcpClient*);

    private slots:
        void readyRead();
        void disconnect();

    };
}

#endif // TCPCLIENT_H
