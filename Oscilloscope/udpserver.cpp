#include <QNetworkDatagram>
#include <QDataStream>

#include "udpserver.h"
#include "frameparser.h"

namespace oscilloscope {
    /// КОНСТРУКТОР ПО УМОЛЧАНИЮ

    UdpServer::UdpServer(quint16 port, QObject *parent) : Server(port, parent) {}

    /// СТАР UDP СЕРВЕРА

    bool UdpServer::start() {
        if (_socket.bind(QHostAddress::Any, port())) {
            connect(&_socket, SIGNAL(readyRead()), this, SLOT(read()));
            return true;
        }

        return false;
    }

    /// ВЫКЛЮЧЕНИЕ СЕРВЕРА

    void UdpServer::stop() {
        _socket.disconnectFromHost();
        disconnect(&_socket);
    }

    /// ЧТЕНИЕ ВХОДНЫХ ДАННЫХ И ФОРМИРОВАНИЕ КАДРА

    void UdpServer::read() {
        while (_socket.hasPendingDatagrams()) {
            QNetworkDatagram datagram = _socket.receiveDatagram();
            QByteArray data = datagram.data();

            if (data.size() < 4) continue;

            QDataStream stream(data);
            stream.setByteOrder(QDataStream::LittleEndian);

            quint32 frameSize;
            stream >> frameSize;

            if (data.size() < static_cast<int>(frameSize))
                continue;

            data = data.right(data.size() - 4);
            Frame *newFrame = FrameParser::parse(data);

            emit frame(newFrame);
        }
    }
}
