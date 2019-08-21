///     СПИСОК ЛОКАЛЬНЫХ (ДУБЛИКАТОВ) КАНАЛОВ И СОЗДАНИЕ ИХ ВЫВОДА

#ifndef LOCALCHANNELLIST_H
#define LOCALCHANNELLIST_H

#include <QLayout>
#include <QObject>

#include "channellist.h"
#include "localchannellistview.h"
#include "dublicatechannel.h"

namespace oscilloscope {
    class LocalChannelList : public iChannelList {
        Q_OBJECT

    public:
        explicit LocalChannelList(QLayout *parent = 0);

        void add(DublicateChannel *channel);
        void dublicatesDelete(const QString &nameParent);

        int indexByName(const QString &name) const;

        LocalChannelListView *channelsView() const;

        ~LocalChannelList();

    private:
        LocalChannelListView *_channelsView;
        int _countDublicates;

    private slots:
        void itemCheck(QObject *);

    signals:
        void itemChecked(QObject *);
    };
}

#endif
