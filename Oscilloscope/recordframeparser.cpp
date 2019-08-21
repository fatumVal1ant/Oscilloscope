#include "recordframeparser.h"

#include <QDataStream>

namespace oscilloscope {
    /// КОНСТРУКТОР

    RecordFrameParser::RecordFrameParser(const QString &filename, QObject *parent) : QObject(parent) {
        _milliPeriod = 1000;
        _timer = new QTimer(this);
        connect(_timer, SIGNAL(timeout()), this, SLOT(trigger()));
        _file = new QFile(filename);
    }

    /// ГЕТТЕР СКОРОСТИ ВОСПРОИЗВЕДЕНИЯ

    int RecordFrameParser::milliPeriod() const {
        return _milliPeriod;
    }

    /// ГЕТТЕР ИМЕНИ КАНАЛА

    const QString RecordFrameParser::channelName() const {
        return _channelName;
    }

    /// ИНИЦИАЛИЗАЦИЯ ПАРСЕРА ЗАПИСИ

    bool RecordFrameParser::init() {
        if (!_file->open(QIODevice::ReadOnly)) {
            emit error("Ошибка: " + QString::number(_file->error()));

            return false;
        }

        QDataStream stream(_file);
        stream.setByteOrder(QDataStream::LittleEndian);

        _channelName = "Запись: " + FrameParser::readString(stream);
        _file->close();

        return true;
    }

    /// СТАРТ ЧТЕНИЯ ЗАПИСИ

    void RecordFrameParser::start() {
        if (!_file->open(QIODevice::ReadOnly)) {
            emit error("Ошибка: " + QString::number(_file->error()));

            return;
        }

        QDataStream stream(_file);
        stream.setByteOrder(QDataStream::LittleEndian);
        _channelName = "Запись: " + FrameParser::readString(stream);

        _timer->setInterval(_milliPeriod);
        _timer->start();
    }

    /// ТРИГГЕР СОЗДАНИЯ КАДРА

    void RecordFrameParser::trigger() {
        if (_file->atEnd()) {
            stop();
            return;
        }

        Frame *newFrame = new Frame();
        newFrame->_channelName = _channelName;

        QDataStream stream(_file); stream.setByteOrder(QDataStream::LittleEndian);

        newFrame->_xMeasure = FrameParser::readString(stream);
        newFrame->_yMeasure = FrameParser::readString(stream);

        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

        stream >> newFrame->_divXValue;
        stream >> newFrame->_divYValue;

        quint32 N;
        stream >> N;

        qint32 offsetX;
        stream >> offsetX;

        newFrame->_offsetX.push_back(offsetX * static_cast<double>(newFrame->_divXValue));

        stream >> newFrame->_time;

        quint32 isComplex;
        stream >> isComplex;
        newFrame->_isComplex = (!(isComplex == 0));
        stream.setFloatingPointPrecision(QDataStream::DoublePrecision);

        FrameParser::readPoints<double>(stream, N, newFrame->_points, newFrame->_isComplex);

        for (int i = 1; i < static_cast<int>(N); i++)
            newFrame->_offsetX.push_back(newFrame->_offsetX.at(i - 1) + static_cast<double>(newFrame->_divXValue));

        emit frame(newFrame);
    }

    /// ОСТАНОВКА ЧТЕНИЯ ЗАПИСИ ИЗ ФАЙЛА

    void RecordFrameParser::stop() {
        _timer->stop();

        if (_file->isOpen()) _file->close();
    }

    /// ИЗМЕНЕНИЕ СКОРОСТИ ВОПСПРОИЗВЕДЕНИЯ ЗАПИСИ

    void RecordFrameParser::setMilliPeriod(int milliPeriod) {
        _milliPeriod = milliPeriod;
        _timer->setInterval(milliPeriod);
    }
}
