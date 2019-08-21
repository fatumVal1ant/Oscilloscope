#ifndef RECORDFRAMEPARSER_H
#define RECORDFRAMEPARSER_H

#include <QObject>
#include <QFile>
#include <QTimer>

#include "frame.h"
#include "frameparser.h"

namespace oscilloscope {

    class RecordFrameParser : public QObject {
        Q_OBJECT

    private:
        QFile *_file;
        int _milliPeriod;
        QTimer *_timer;
        QString _channelName;

    public:
        explicit RecordFrameParser(const QString &filename, QObject *parent = nullptr);

        int milliPeriod() const;
        const QString channelName() const;

        bool init();

    signals:
        void error(const QString &errorString);
        void frame(Frame *frame);

    public slots:
        void start();
        void stop();

        void setMilliPeriod(int milliPeriod);

    private slots:
        void trigger();

    };
}
#endif // RECORDFRAMEPARSER_H
