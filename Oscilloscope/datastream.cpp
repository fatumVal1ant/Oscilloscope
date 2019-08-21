#include "datastream.h"

namespace oscilloscope {
    /// КОНСТРУКТОР СОЗДАНИЯ ДАННЫХ НА ОСНОВЕ ПОЛУЧЕННОГО КАДРА

    DataStream::DataStream(Frame *frame) {
        _frame = frame;
    }

    /// ОБНОВЛЕНИЕ КАДРА

    void DataStream::update(Frame *frame) {
        emit receivedFrame(frame);

        if (_frame != nullptr)
            delete _frame;

        _frame = frame;

        emit update();
    }

    void DataStream::insert(Frame *frame) {
        emit receivedFrame(frame);

        for (int i = 0; i < _frame->_points.length(); i++) {
            if (_frame->_offsetX.at(i) >= frame->_offsetX.at(0)) {
                for (int j = i, k = 0; k < frame->_points.length(); j++, k++) {
                    _frame->_offsetX.insert(j, frame->_offsetX.at(k));
                    _frame->_points.insert(j, frame->_points.at(k));
                }

                delete frame;

                return;
            }
        }

        _frame->_offsetX.append(frame->_offsetX);
        _frame->_points.append(frame->_points);

        delete frame;
    }

    /// ПОЛУЧЕНИЕ КАДРА

    Frame *DataStream::frame() const {
        return _frame;
    }

    /// ДЕСТРУКТОР

    DataStream::~DataStream() {
        if (_frame) delete _frame;
    }
}

