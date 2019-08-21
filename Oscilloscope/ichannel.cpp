#include "ichannel.h"

#include "kiss_fft.h"
#include "signalsmoothing.h"
#include "trigger.h"

#include <complex>

namespace oscilloscope {
    /// СОЗДАНИЕ КАНАЛА НА ОСНОВЕ ПОЛУЧЕННЫХ СТРУКТУРИРОВАННЫХ ДАННЫХ

    iChannel::iChannel(DataStream *data) {
        _data = data;
    }

    /// СОЗДАНИЕ КАНАЛА (ДУБЛИКАТА) НА ОСНОВЕ УЖЕ СУЩЕСТВУЮЩЕГО КАНАЛА

    iChannel::iChannel(const iChannel *channel) : iChannel(channel->data()) {}

    /// ПОЛУЧЕНИЕ ТИПА КАНАЛА (ДУБЛИКАТ ИЛИ ОРИГИНАЛ)

    int iChannel::channelType() const {
        return _channelType;
    }

    /// ПОЛУЧЕНИЕ ДАННЫХ КАНАЛА

    DataStream *iChannel::data() const {
        return _data;
    }

    /// ПОЛУЧЕНИЕ МАССИВА ТОЧЕК

    QVector<std::complex<double>> iChannel::points() const {
        return _points;
    }

    QVector<double> iChannel::offsetX() const {
        return _offsetX;
    }

    /// ПРЕОБРАЗОВАНИЕ ДАННЫХ В НУЖНЫЙ НАМ ВИД

    void iChannel::trigger(Enums::TriggersType type, double level) {
        if (type == Enums::TriggersType::TriggerByForwardFront) _points = triggerByForwardFront(_data->frame()->_points, level);
        else if (type == Enums::TriggersType::TriggerByBackFront) _points = triggerByBackFront(_data->frame()->_points, level);
        else if (type == Enums::TriggersType::TriggerByTime) _points = triggerByTime(_data->frame()->_points, level, _data->frame()->_divXValue);
        else _points = _data->frame()->_points;

        _offsetX = _data->frame()->_offsetX;
    }

    void iChannel::transform(Enums::TransformateType type, double expSmthCoef, int movingAvgCoef) {
        if (type == Enums::TransformateType::BPF) {
            QVector<QVector<std::complex<double>>> vector = bpf(_points, _offsetX, _data->frame()->_divXValue);

            _points = vector.at(0);

            _offsetX.clear();

            for (int i = 0; i < vector.at(1).size(); i++) {
                _offsetX.push_back(vector.at(1).at(i).real());
            }

            return;
        }
        else if (type == Enums::TransformateType::ThreePointFilter) _points = threePointFilter(_points);
        else if (type == Enums::TransformateType::ExponentialSmoothing) _points = expSmoothing(_points, expSmthCoef);
        else if (type == Enums::TransformateType::MovingAverage) _points = movingAvg(_points, movingAvgCoef);
    }

    /// ДЕСТРУКТОР

    iChannel::~iChannel() {}

    /// ВНЕШНЯЯ ФУНКЦИЯ ПРЕОБРАЗОВАНИЯ БПФ

    QVector<QVector<std::complex<double>>> bpf(const QVector<std::complex<double>> &p, const QVector<double> &x, double step) {
        QVector<QVector<std::complex<double>>> vector;

        QVector<std::complex<double>> points;
        QVector<std::complex<double>> offsetX;

        if (p.length() <= 0) {
            vector.push_back(points);
            vector.push_back(offsetX);

            return vector;
        }

        points = FFT::goFFT(p);

        for (int i = 0; i < x.size() * step; i++)
            offsetX.push_back(x.at(i) / (points.size() * step));

        for (int i = 0; i < points.size(); i++) {
            points[i].real(sqrt(points[i].real() * points[i].real() + points[i].imag() * points[i].imag()));
            points[i].imag(0);
        }

        vector.push_back(points);
        vector.push_back(offsetX);

        return vector;
    }

    /// ВНЕШНЯЯ ФУНКЦИЯ ТРЕХТОЧЕЧНОГО ФИЛЬТРА

    QVector<std::complex<double>> threePointFilter(const QVector<std::complex<double> > &points) {
        return SignalSmoothing::threePoints(points);
    }

    /// ВНЕШНЯЯ ФУНКЦИЯ ЭКСПОНЕНЦИАЛЬНОГО СГЛАЖИВАНИЯ

    QVector<std::complex<double>> expSmoothing(const QVector<std::complex<double> > &points, double coef) {
        return SignalSmoothing::expFilter(points, coef);
    }

    /// ВНЕШНЯЯ ФУНКЦИЯ СКОЛЬЗЯЩЕГО СРЕДНЕГО

    QVector<std::complex<double>> movingAvg(const QVector<std::complex<double> > &points, int coef) {
        return SignalSmoothing::movAverage(points, coef);
    }

    QVector<std::complex<double>> triggerByBackFront(const QVector<std::complex<double> > &points, double level) {
        return Trigger::triggerByBackFront(points, level);
    }

    QVector<std::complex<double>> triggerByForwardFront(const QVector<std::complex<double> > &points, double level) {
        return Trigger::triggerByForwardFront(points, level);
    }

    QVector<std::complex<double>> triggerByTime(const QVector<std::complex<double> > &points, double level, double step) {
        return Trigger::triggerByTime(points, level, step);
    }
}

