///     РОДИТЕЛЬ КАНАЛОВ (ДУБЛИКАТОВ И ОРИГИНАЛОВ)

#ifndef ICHANNEL_H
#define ICHANNEL_H

#define CH_ORIGINAL 0
#define CH_DUBLICATE 1

#include <QVector>
#include <QString>
#include <QObject>

#include "datastream.h"
#include "enums.h"

namespace oscilloscope {
    class iChannel : public QObject {
        Q_OBJECT
    public:
        int channelType() const;
        DataStream *data() const;

        QVector<std::complex<double>> points() const;
        QVector<double> offsetX() const;

        void trigger(Enums::TriggersType type = Enums::TriggersType::WithoutTriggers, double level = 0.0);
        void transform(Enums::TransformateType type = Enums::TransformateType::None, double expSmthCoef = 0.0, int movingAvgCoef = 0);

        ~iChannel();

    private:
        QVector<std::complex<double>> _points;
        QVector<double> _offsetX;

    protected:
        explicit iChannel(DataStream *data);
        iChannel(const iChannel *channel);

        int _channelType;
        DataStream *_data;
    };

    /// ПРЕОБРАЗОВАНИЯ

    QVector<QVector<std::complex<double>>> bpf(const QVector<std::complex<double>> &points, const QVector<double> &x, double step);
    QVector<std::complex<double>> threePointFilter(const QVector<std::complex<double>> &points);
    QVector<std::complex<double>> expSmoothing(const QVector<std::complex<double>> &points, double coef);
    QVector<std::complex<double>> movingAvg(const QVector<std::complex<double>> &points, int coef);

    /// ТРИГГЕРЫ

    QVector<std::complex<double>> triggerByForwardFront(const QVector<std::complex<double>> &points, double level);
    QVector<std::complex<double>> triggerByBackFront(const QVector<std::complex<double>> &points, double level);
    QVector<std::complex<double>> triggerByTime(const QVector<std::complex<double>> &points, double level, double step);
}

#endif
