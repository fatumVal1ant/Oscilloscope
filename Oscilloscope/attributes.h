///     АТРИБУТЫ ЛОКАЛЬНОГО КАНАЛА ДЛЯ ОТРИСОВКА НА ДИСПЛЕЕ

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QColor>

#include "enums.h"

namespace oscilloscope {
    struct Attributes {
        QColor _colorReal, _colorImg;

        int _lineType;
        int _lineWidth;

        double _expSmthCoef;
        int _moveAvgCoef;
        double _triggerLevel;

        bool _showReal, _showImag;

        int _startX, _startY;

        Enums::TransformateType _transformateType;
        Enums::TriggersType _triggerType;

        Attributes();
    };
}

#endif
