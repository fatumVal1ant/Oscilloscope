///     ДУБЛИКАТ КАНАЛА

#ifndef DUBLICATECHANNEL_H
#define DUBLICATECHANNEL_H

#define DUBLICATE_NAME(index, name) "D " + QString::number(index) + " (" + name + ")"
#define DUBLICATE_NAME_BY_PARENT(name) "(" + name + ")"
#define DEF_DUBLICATE_NAME "Неопознанный дубликат"

#include "ichannel.h"
#include "channel.h"

namespace oscilloscope {
    class DublicateChannel : public iChannel {
    public:
        explicit DublicateChannel(const Channel *channel);

        const QString name() const;
        const QString setName(const QString &name);

    private:
        QString _name;
    };
}

#endif
