#include <QDataStream>

#include "tcpclient.h"
#include "frameparser.h"

namespace oscilloscope {
    /// КОНСТРУКТОР ТСП КЛИЕНТА

    TcpClient::TcpClient(QTcpSocket* socket, QObject* parent) : QObject (parent) {
        _socket = socket;

        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    }

    /// ЧТЕНИЕ ДАННЫХ ПО СОКЕТУ

    Frame *TcpClient::read() {
        static quint32 frameSize = 0;

        if (frameSize == 0) {
            if (_socket->bytesAvailable() < 4) return nullptr;
                else {
                    QDataStream stream(_socket->read(4));
                    stream.setByteOrder(QDataStream::LittleEndian);
                    stream >> frameSize;
                }
        }

        if (frameSize > 4) {
            if (_socket->bytesAvailable() < frameSize - 4) return nullptr;
                else {
                    QByteArray data = _socket->read(frameSize - 4);
                    frameSize = 0;

                    return FrameParser::parse(data);
                }
        }

        if (frameSize <= 4) frameSize = 0;

        return nullptr;
    }

    /// СЛОТ ГОТОВНОСТИ ЧТЕНИЯ

    void TcpClient::readyRead() {
        emit readyRead(this);
    }

    /// СЛОТ ОТКЛЮЧЕНИЯ КЛИЕНТА

    void TcpClient::disconnect() {
        delete this;
    }

    /// ДЕСТРУКТОР

    TcpClient::~TcpClient() {
        if (_socket->isOpen()) _socket->close();
    }
}
