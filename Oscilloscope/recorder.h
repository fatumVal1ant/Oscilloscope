#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QThread>
#include <QBuffer>

#include "datastream.h"

namespace oscilloscope {
    class RecorderThread : public QObject {
    Q_OBJECT

    private:
        bool _isInterrupted;
        QBuffer *_buffer;
        DataStream *_dataStream;

    private slots:
        void receiveFrame(Frame *);

    public:
        explicit RecorderThread(DataStream *dataStream);

        void stop();
        bool isRecorded() const;
        void save(const QString &filename);

    public slots:
        void process();

    signals:
        void stopped();
        void error(const QString &errorMessage);

    };

    class Recorder : public QObject {
        Q_OBJECT

    private:
        QThread *_thread;
        RecorderThread *_recorderThread;

    public:
        explicit Recorder(DataStream *dataStream, QObject *parent = nullptr);

        bool isRecorded() const;

        ~Recorder();

    signals:
        void error(const QString &errorMessage);

    public slots:
        void startRecord();
        void stopRecord();
        void saveRecord(const QString &filename);

    };
}
#endif // RECORDER_H
