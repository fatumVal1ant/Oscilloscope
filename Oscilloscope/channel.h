///     ОРИГИНАЛЬНЫЙ КАНАЛ

#ifndef CHANNEL_H
#define CHANNEL_H

#include <QTimer>
#include <QObject>

#include "ichannel.h"

namespace oscilloscope {
    class Channel : public iChannel {
        Q_OBJECT

    private:
        QTimer *_invalidTimer;
        bool _alive;

    public:
        explicit Channel(DataStream *data);

        bool status() const;

        ~Channel();

    public slots:
        void channelDisconnected();
        void channelConnected();

    signals:
        void channelUpdated(Channel *);
    };
}

#endif
