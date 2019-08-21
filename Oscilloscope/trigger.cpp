#include "trigger.h"

namespace oscilloscope {
    /// ТРИГГЕР ПО ПЕРЕДНЕМУ ФРОНТУ

    QVector<std::complex<double>> Trigger::triggerByForwardFront(const QVector<std::complex<double>> &data, double level) {
        QVector<std::complex<double>> out;

        for (int i = 1; i < data.size(); i++) {
            if ((data[i].real() - data[i - 1].real()) > 0) {
                if (data[i].real() >= level){
                    for (int j = i; j < data.size(); j++)
                        out.push_back(data[j].real());

                    break;
                }
            }
        }

        double lastY = 0;
        for (int i = 1; i < data.size(); i++) {
            if ((data[i].imag() - data[i - 1].imag()) > 0) {
                if (data[i].imag() >= level) {
                    for (int j = i, k = 0; j < data.size(); j++, k++) {
                        if (out.size() > k) {
                            out[k] = std::complex<double>(out[k].real(), data[j].imag());
                            lastY = out[k].real();
                        } else out.push_back(std::complex<double>(lastY, data[j].imag()));
                    }

                    break;
                }
            }
        }

        return out;
    }

    /// ТРИГГЕР ПО ЗАДНЕМУ ФРОНТУ

    QVector<std::complex<double>> Trigger::triggerByBackFront(const QVector<std::complex<double>> &data, double level) {
        QVector<std::complex<double>> out;

        for (int i = 0; i < data.size() - 1; i++) {
            if ((data[i + 1].real() - data[i].real()) < 0) {
                if (data[i].real() >= level) {
                    for (int j = i; j < data.size(); j++)
                        out.push_back(data[j].real());

                    break;
                }
            }
        }

        double lastY = 0;
        for (int i = 0; i < data.size() - 1; i++) {
            if ((data[i + 1].real() - data[i].real()) < 0) {
                if (data[i].real() >= level) {
                    for (int j = i, k = 0; j < data.size(); j++, k++) {
                        if (out.size() > k) {
                            out[k] = std::complex<double>(out[k].real(), data[j].imag());
                            lastY = out[k].real();
                        } else out.push_back(std::complex<double>(lastY, data[j].imag()));
                    }

                    break;
                }
            }
        }

        return out;
    }

    /// ТРИГГЕР ПО ВРЕМЕНИ

    QVector<std::complex<double>> Trigger::triggerByTime(const QVector<std::complex<double>> &data, double time, double step) {
        QVector<std::complex<double>> out;

        int S = data.size(), count = 0;
        double i = 0;

        while (i < time) {
            i += step;
            count++;
        }

        for (int j = count; j < S; j++)
            out.push_back(data[j]);

        return out;
    }
}
