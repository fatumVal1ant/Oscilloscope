#ifndef MARKERMEASUREMENTS_H
#define MARKERMEASUREMENTS_H

#include <QtCharts/QtCharts>
#include <QVector>
#include <complex>

#include "marker.h"

namespace oscilloscope {
    class MarkerMeasurements {
    public:
        // Математическое ожиание
        static const std::complex<double> expectedValue(const QVector<double> &, const QVector<std::complex<double>> &, const QList<Marker *> &);

        // Среднеквадратическое отклонение
        static const std::complex<double> standardDeviation(const QVector<double> &, const QVector<std::complex<double>> &, const QList<Marker *> &, std::complex<double> measur);

        // Мощность
        static const std::complex<double> signalCapacity(const QVector<double> &, const QVector<std::complex<double>> &, const QList<Marker *> &);

        // Амплитуда
        static const std::complex<double> signalAmplitude(const QVector<QPointF *> &);

        // Поиск экстремумов
        static const QVector<QPointF *> minAndMax(const QVector<double> &, const QVector<std::complex<double>> &, const QList<Marker *> &);

        // Комплексная амплитуда
        static double complexAmplitude(const QVector<double> &, const QVector<std::complex<double>> &, const QList<Marker*> &);
    };
}

#endif // MARKERMEASUREMENTS_H
