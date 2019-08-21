///     СТРУКТУРА ПОЛУЧАЕМОГО КАДРА ДАННЫХ

#ifndef FRAME_H
#define FRAME_H

#include <QString>
#include <QVector>
#include <complex>

namespace oscilloscope {
    struct Frame {
        quint8 _frameNumber;

        QString _channelName;

        QString _xMeasure;
        QString _yMeasure;

        float _divXValue;
        float _divYValue;

        QVector<double> _offsetX;

        qint32 _time;

        bool _isComplex;
        bool _isFloat;

        quint8 _pointSize;

        QVector<std::complex<double>> _points;
    };
}

#endif
