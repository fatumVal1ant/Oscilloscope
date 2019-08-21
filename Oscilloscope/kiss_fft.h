#ifndef FFT_H
#define FFT_H

#include <complex>
#include <QtMath>
#include <QVector>
#include <QVariant>

class FFT {
    public:
        static QVector<std::complex<double>> goFFT(const QVector<std::complex<double>> &data);
};


#endif // FFT_H
