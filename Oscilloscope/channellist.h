///     РОДИТЕЛЬ СПИСКОВ ОРИГИНАЛОВ И ДУБЛИКАТОВ КАНАЛОВ

#ifndef CHANNELLIST_H
#define CHANNELLIST_H

#define CH_ALIVE Qt::black
#define CH_DEAD Qt::gray

#include <QList>
#include <QObject>

#include "ichannel.h"

namespace oscilloscope {
    class iChannelList : public QObject {
        Q_OBJECT

    protected:       
       QList<iChannel *> *_channels;

       iChannelList();

    public:        
        QList<iChannel *> *channels() const;

        void add(iChannel *channel);
        virtual int indexByName(const QString &name) const = 0;

        ~iChannelList();

    protected slots:
        void channelDelete(const QString &name);

    signals:
        void channelDeleted(const QString &name);

    };
}

#endif
