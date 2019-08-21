#include "frameparser.h"

#include <QDataStream>
#include <QIODevice>
#include <complex>

namespace oscilloscope {
    /// ПАРСИНГ ПОЛУЧЕННЫХ ДАННЫХ

    Frame *FrameParser::parse(QByteArray &data) {
        Frame *frame = new Frame();

        QDataStream stream(data);
        stream.setByteOrder(QDataStream::LittleEndian);

        stream.skipRawData(4);                  // Версия протокола
        stream.skipRawData(2);                  // Reserv

        stream >> frame->_frameNumber;

        quint8 frameType;
        stream >> frameType;
        if (frameType != 1) {
            delete frame;
            return nullptr;
        }

        frame->_channelName = readString(stream);

        frame->_xMeasure = readString(stream);
        frame->_yMeasure = readString(stream);

        stream.setFloatingPointPrecision(QDataStream::FloatingPointPrecision::SinglePrecision);

        stream >> frame->_divXValue;
        stream >> frame->_divYValue;

        quint32 N;
        stream >> N;

        quint32 offsetX;
        stream >> offsetX;
        frame->_offsetX.push_back(offsetX * frame->_divXValue);

        stream >> frame->_time;

        qint8 isBigEndian;
        stream >> isBigEndian;

        if (static_cast<bool>(isBigEndian)) stream.setByteOrder(QDataStream::BigEndian);
            else stream.setByteOrder(QDataStream::LittleEndian);

        quint8 isComplex;
        stream >> isComplex;
        frame->_isComplex = static_cast<bool>(isComplex);

        quint8 isFloat;
        stream >> isFloat;
        frame->_isFloat = static_cast<bool>(isFloat);

        quint8 pointSize;
        stream >> pointSize;
        frame->_pointSize = pointSize;

        if (frame->_isFloat) {
            if (pointSize == 4) readPoints<float>(stream, N, frame->_points, frame->_isComplex);
            else if (pointSize == 8) {
                stream.setFloatingPointPrecision(QDataStream::FloatingPointPrecision::DoublePrecision);
                readPoints<double>(stream, N, frame->_points, frame->_isComplex);
            }
        } else {
            if (pointSize == 1) readPoints<qint8>(stream, N, frame->_points, frame->_isComplex);
            else if (pointSize == 2) readPoints<quint16>(stream, N, frame->_points, frame->_isComplex);
            else if (pointSize == 4) readPoints<quint32>(stream, N, frame->_points, frame->_isComplex);
            else if (pointSize == 8) readPoints<quint64>(stream, N, frame->_points, frame->_isComplex);
        }

        for (int i = 1; i < (int)N; i++)
            frame->_offsetX.push_back(frame->_offsetX.at(i - 1) + frame->_divXValue);


        return frame;
    }

    /// СЧИТЫВАНИЕ ФОРМАТА STRING ИЗ ПОТОКА

    QString FrameParser::readString(QDataStream &stream) {
        quint8 strSize;
        stream >> strSize;

        if (strSize != 0) {
            QString result;

            for (int i = 0; i < strSize; i++) {
                quint8 character;
                stream >> character;
                result.append(QChar(character));
            }

            if ((strSize + 1) % 4 != 0) {
                stream.skipRawData(4 - ((strSize + 1) % 4));
            }

            return result;
        } else {
            stream.skipRawData(3);

            return QString();
        }
    }

    /// СЧИТЫВАНИЕ ТОЧЕК

    template<typename T>
    void FrameParser::readPoints(QDataStream &stream, uint N, QVector<std::complex<double>> &points, bool complex) {
        if (complex) {
            T realPoint;
            T imagPoint;

            for (uint i = 0; i < N; ++i) {
                stream >> realPoint;
                stream >> imagPoint;

                std::complex<double> point(realPoint, imagPoint);

                points.push_back(point);
            }
        } else {
            T point;

            for (uint i = 0; i < N; ++i) {
                stream >> point;
                points.push_back(point);
            }
        }
    }
}
