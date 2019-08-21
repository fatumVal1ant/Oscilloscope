///     ПОТОК СТРУКТУРИРОВАННЫХ ДАННЫХ

#ifndef DATASTREAM_H
#define DATASTREAM_H

#include <QObject>

#include "frame.h"

namespace oscilloscope {
    class DataStream : public QObject {
        Q_OBJECT

        private:
            Frame *_frame;

        public:
            explicit DataStream(Frame *frame);

            Frame *frame() const;
            void update(Frame *);
            void insert(Frame *);

            ~DataStream();

        signals:
            void update();
            void receivedFrame(Frame *frame);
    };
}

#endif
