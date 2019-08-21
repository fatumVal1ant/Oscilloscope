#ifndef SIGNALSMOOTHING_H
#define SIGNALSMOOTHING_H

#include <QVector>
#include <complex>

namespace oscilloscope {
    class SignalSmoothing {
    public:
        static QVector<std::complex<double>> threePoints(const QVector<std::complex<double>> &data);                // Трехточечный фильтр
        static QVector<std::complex<double>> expFilter(const QVector<std::complex<double>> &data, double coef);     // Экспоненциальное сглаживание
        static QVector<std::complex<double>> movAverage(const QVector<std::complex<double>> &data, int order);      // Скользящее среднее
    };
}

#endif // SIGNALSMOOTHING_H
