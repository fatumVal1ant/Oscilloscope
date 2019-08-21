#ifndef FRAMEPARSER_H
#define FRAMEPARSER_H

#include "frame.h"

namespace oscilloscope {
    class FrameParser {
    public:
        static Frame *parse(QByteArray &data);
        static QString readString(QDataStream &);

        template<typename T>
        static void readPoints(QDataStream &, uint, QVector<std::complex<double>> &, bool complex = false);
    };
}

#endif // FRAMEPARSER_H
