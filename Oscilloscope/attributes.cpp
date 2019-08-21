#include "attributes.h"

namespace oscilloscope {
    /// Конструктор атрибутов

    Attributes::Attributes() {
        _colorReal = Qt::black;
        _colorImg = Qt::black;

        _lineType = 0;
        _lineWidth = 1;

        _expSmthCoef = 0.01;
        _moveAvgCoef = 1;
        _triggerLevel = 0;

        _showReal = true;
        _showImag = false;

        _startX = 0;
        _startY = 0;

        _transformateType = Enums::TransformateType::None;
        _triggerType = Enums::TriggersType::WithoutTriggers;
    }
}
