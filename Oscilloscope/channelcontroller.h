///     ОБРАБОТЧИК ПОЛУЧЕННЫХ ДАННЫХ

#ifndef CHANNELCONTROLLER_H
#define CHANNELCONTROLLER_H

#include <QObject>

#include "server.h"
#include "frame.h"
#include "globalchannellist.h"
#include "recordframeparser.h"

namespace oscilloscope {
    class ChannelController : public QObject {
      Q_OBJECT

    private:
        Server *_tcpServer;
        Server *_udpServer;

        GlobalChannelList *_globalChannelList;

    public:
        explicit ChannelController(GlobalChannelList *channels);

        bool createTcpServer(quint16 port);
        bool createUdpServer(quint16 port);

        void addRecordFrameParser(RecordFrameParser *parser);

    private slots:
        void receiveFrame(Frame *);

    public slots:
        void reloadTcpServer();
        void reloadUdpServer();

    signals:
        void channelUpdated(const QString &name);
    };
}

#endif
