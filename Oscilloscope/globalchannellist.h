///     СПИСОК ОРИГИНАЛЬНЫХ КАНАЛОВ И СОЗДАНИЕ ИХ ВЫВОДА

#ifndef GLOBALCHANNELLIST_H
#define GLOBALCHANNELLIST_H

#include <QLayout>
#include <QObject>

#include "channellist.h"
#include "globalchannellistview.h"
#include "channel.h"

namespace oscilloscope {
    class GlobalChannelList : public iChannelList {
        Q_OBJECT

    public:
        explicit GlobalChannelList(QLayout *parent = 0);

        void add(iChannel *channel, bool alive = true);
        int indexByName(const QString &name) const;
        QListWidgetItem *getSelectedItem() const;
        iChannel *getSelectedChannel() const;

        ~GlobalChannelList();

    public slots:
        void channelUpdate(Channel *);
        void currentItemChange(QListWidgetItem *oldItem, QListWidgetItem *newItem);

    private:
        GlobalChannelListView *_channelsView;

    signals:
        void channelUpdated(const QString &name);
        void currentItemChanged(QListWidgetItem *oldItem, QListWidgetItem *newItem);

    };
}

#endif
