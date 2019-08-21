#include "channellist.h"

#include <QDebug>

namespace oscilloscope {
    /// Конструктор - создание списка каналов

    iChannelList::iChannelList() {
        _channels = new QList<iChannel *>;
    }

    /// Получения списка каналов

    QList<iChannel *> *iChannelList::channels() const {
        return _channels;
    }

    /// Добавление канала в список

    void iChannelList::add(iChannel * channel) {
        _channels->push_back(channel);
    }

    /// Удаление канала

    void iChannelList::channelDelete(const QString &name) {
        emit channelDeleted(name);

        int index = indexByName(name);

        if (index >= 0) {
            delete _channels->at(index);
            _channels->removeAt(index);
        }
    }

    /// Деструктор

    iChannelList::~iChannelList() {
        for (int i = 0; i < _channels->length(); i++)
            delete _channels->at(i);

        delete _channels;
    }
}
