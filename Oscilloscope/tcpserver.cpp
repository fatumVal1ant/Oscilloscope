#include <QDataStream>

#include "tcpserver.h"

namespace oscilloscope {
    /// КОНСТРУКТОР ПО УМОЛЧАНИЮ

    TcpServer::TcpServer(quint16 port, QObject* parent) : Server(port, parent) {
        _server = new QTcpServer(this);
        connect(_server, SIGNAL(newConnection()), this, SLOT(receiveConnection()));
    }

    /// ДЕСТРУКТОР

    TcpServer::~TcpServer() {
        stop();
    }

    /// ЗАПУСК ТСП СЕРВЕРА

    bool TcpServer::start() {
        return _server->listen(QHostAddress::Any, port());;
    }

    /// ВЫКЛЮЧЕНИЕ ТСП СЕРВЕРА

    void TcpServer::stop() {
        _server->close();
    }

    /// ПРИНЯТИЕ НОВОГО ПОДКЛЮЧЕНИЯ

    void TcpServer::receiveConnection() {
        QTcpSocket *socket = _server->nextPendingConnection();

        TcpClient *client = new TcpClient(socket, this);
        connect(client, SIGNAL(readyRead(TcpClient *)), this, SLOT(receiveData(TcpClient *)));
    }

    /// ПОЛУЧЕНИЕ ВХОДНЫХ ДАННЫХ И ФОРМИРОВАНИЕ НОВОГО КАДРА

    void TcpServer::receiveData(TcpClient *client) {
        Frame *newFrame = client->read();

        if (newFrame != nullptr)
            emit frame(newFrame);
    }
}
