///     РОДИТЕЛЬ СЕРВЕРОВ

#ifndef SERVER_H
#define SERVER_H

#include <QObject>

#include "frame.h"

namespace oscilloscope {
    class Server : public QObject {
        Q_OBJECT

    private:
        quint16 _port;

    protected:
        quint16 port();

    public:
        explicit Server(quint16 port, QObject *parent = nullptr);

        virtual bool start() = 0;
        virtual void stop() = 0;

    signals:
        void frame(Frame *);

    };
}

#endif // SERVER_H
