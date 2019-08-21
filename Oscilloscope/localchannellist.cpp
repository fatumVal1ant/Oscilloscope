#include "localchannellist.h"
#include "localchannellistview.h"

#include "dublicatechannel.h"

#include <QDebug>

namespace oscilloscope {
    /// Конструктор создания локального списка с указанием разменщения его View`ера

    LocalChannelList::LocalChannelList(QLayout *parent) : iChannelList() {
        _countDublicates = 1;

        _channelsView = new LocalChannelListView();

        connect(_channelsView, SIGNAL(channelDeleted(const QString)), this, SLOT(channelDelete(const QString)));
        connect(_channelsView, SIGNAL(itemChecked(QObject *)), this, SLOT(itemCheck(QObject *)));

        if (parent) parent->addWidget(_channelsView);
            else _channelsView->show();
    }

    /// Добавление дубликата в список и его отображение

    void LocalChannelList::add(DublicateChannel *channel) {
        iChannelList::add(channel);

        channel->setName(DUBLICATE_NAME(_countDublicates++, channel->data()->frame()->_channelName));
        _channelsView->addChannel(channel->name());
    }

    /// Получение указателя на View`ер

    LocalChannelListView *LocalChannelList::channelsView() const {
        return _channelsView;
    }

    /// Получение индекса по имени дубликата

    int LocalChannelList::indexByName(const QString &name) const {
        for (int i = 0; i < _channels->length(); i++) {
            DublicateChannel *channel = dynamic_cast<DublicateChannel *>(_channels->at(i));

            if (channel->name() == name)
                return i;
        }

        return -1;
    }

    void LocalChannelList::itemCheck(QObject *object) {
        emit itemChecked(object);
    }

    /// Удаление дубликатов из списка по имени оригинального канала

    void LocalChannelList::dublicatesDelete(const QString &nameParent) {
        for (int i = 0; i < _channels->size(); i++) {
            if (_channels->at(i)->data()->frame()->_channelName == nameParent) {
                delete _channels->at(i);
                _channels->removeAt(i--);
            }
        }
    }

    /// Деструктор

    LocalChannelList::~LocalChannelList() {}
}
