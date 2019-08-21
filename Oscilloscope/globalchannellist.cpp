#include "globalchannellist.h"

namespace oscilloscope {
    /// КОНСТРУКТОР СПИСКА ОРИГИНАЛЬНЫХ КАНАЛОВ

    GlobalChannelList::GlobalChannelList(QLayout *parent) : iChannelList() {
        _channelsView = new GlobalChannelListView();

        connect(_channelsView, SIGNAL(channelDeleted(const QString)), this, SLOT(channelDelete(const QString)));
        connect(_channelsView, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(currentItemChange(QListWidgetItem *, QListWidgetItem *)));

        if (parent) parent->addWidget(_channelsView);
            else _channelsView->show();
    }

    /// ОБРАБОТКА ВЫБОРА ДРУГОГО ОБЪЕКТА

    void GlobalChannelList::currentItemChange(QListWidgetItem *oldItem, QListWidgetItem *newItem) {
        emit currentItemChanged(oldItem, newItem);
    }

    /// ОБНОВЛЕНИЕ ОТОБРАЖЕНИЯ КАНАЛА

    void GlobalChannelList::channelUpdate(Channel *channel) {
        if (channel->status()) _channelsView->item(_channels->indexOf(channel))->setForeground(CH_ALIVE);
            else _channelsView->item(_channels->indexOf(channel))->setForeground(CH_DEAD);

        _channelsView->repaint();

        emit channelUpdated(channel->data()->frame()->_channelName);
    }

    /// ПОЛУЧЕНИЕ ИНДЕКСА ОРИГИНАЛЬНОГО КАНАЛА ПО ЕГО ИМЕНИ

    int GlobalChannelList::indexByName(const QString &name) const {
        for (int i = 0; i < _channels->length(); i++)
            if (_channels->at(i)->data()->frame()->_channelName == name)
                return i;

        return -1;
    }

    /// ПОЛУЧЕНИЕ ВЫБРАННОГО ОБЪЕКТА

    QListWidgetItem *GlobalChannelList::getSelectedItem() const {
        return _channelsView->currentItem();
    }

    /// ПОЛУЧЕНИЕ ВЫБРАННОГО КАНАЛА

    iChannel *GlobalChannelList::getSelectedChannel() const {
        QListWidgetItem *item = _channelsView->currentItem();
        if (!item) return nullptr;

        QString name = item->text();

        return _channels->at(indexByName(name));
    }

    /// ДОБАВЛЕНИЕ ОРИГИНАЛЬНОГО КАНАЛА В СПИСОК

    void GlobalChannelList::add(iChannel *channel, bool alive) {
        iChannelList::add(channel);
        _channelsView->addItem(channel->data()->frame()->_channelName);

        if (alive) _channelsView->item(_channelsView->count() - 1)->setTextColor(CH_ALIVE);
            else _channelsView->item(_channelsView->count() - 1)->setTextColor(CH_DEAD);
    }

    /// ДЕСТРУКТОР

    GlobalChannelList::~GlobalChannelList() {}
}
