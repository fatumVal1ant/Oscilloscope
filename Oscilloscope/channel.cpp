#include "channel.h"

#include <QDebug>

namespace oscilloscope {
    /// Конструктор оригинального канала

    Channel::Channel(DataStream *data) : iChannel(data) {
        _alive = true;
        _channelType = CH_ORIGINAL;

        _invalidTimer = 0;

        _invalidTimer = new QTimer();
        _invalidTimer->start(3600);

        connect(_invalidTimer, SIGNAL(timeout()), this, SLOT(channelDisconnected()));

        _invalidTimer->start();
    }

    /// ПОЛУЧЕНИЕ СТАТУСА КАНАЛА

    bool Channel::status() const {
        return _alive;
    }

    /// Обновление статуса канала при истечении таймера обновлений

    void Channel::channelDisconnected() {
        _invalidTimer->stop();
        _alive = false;

        emit channelUpdated(this);
    }

    void Channel::channelConnected() {
        _invalidTimer->stop();
        _invalidTimer->start(3600);

        _alive = true;

        emit channelUpdated(this);
    }

    /// Деструктор

    Channel::~Channel() {
        if (_invalidTimer) {
            _invalidTimer->stop();
            delete _invalidTimer;
        }

        if (_data) delete _data;
    }
}
