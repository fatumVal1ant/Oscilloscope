#include "server.h"

namespace oscilloscope {
    /// КОНСТРУКТОР СЕРВЕРА

    Server::Server(quint16 port, QObject* parent) : QObject(parent) {
        _port = port;
    }

    /// ГЕТТЕР ПОРТА

    quint16 Server::port() {
        return _port;
    }
}

