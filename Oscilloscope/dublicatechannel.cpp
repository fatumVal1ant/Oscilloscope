#include "dublicatechannel.h"

namespace oscilloscope {
    /// СОЗДАНИЕ ДУБЛИКАТА НА ОСНОВЕ ОРИГИНАЛЬНОГО КАНАЛА

    DublicateChannel::DublicateChannel(const Channel *channel) : iChannel(channel) {
        _channelType = CH_DUBLICATE;
        _name = DEF_DUBLICATE_NAME;

        trigger();
        transform();
    }

    /// ПОЛУЧЕНИЕ ИМЕНИ ДУБЛИКАТА

    const QString DublicateChannel::name() const {
        return _name;
    }

    /// ИЗМЕНЕНИЕ ИМЕНИ ДУБЛИКАТА

    const QString DublicateChannel::setName(const QString &name) {
        return (_name = name);
    }
}
