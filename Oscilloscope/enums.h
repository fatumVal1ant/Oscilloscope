#ifndef ENUMS_H
#define ENUMS_H

#define COLORS_COUNT 5
#define TRANSORM_COUNT 5
#define TRIGGERS_COUNT 4

#include <QObject>

namespace oscilloscope {
    class Enums {
    public:
        enum TransformateType {
            None,
            BPF,
            ThreePointFilter,
            ExponentialSmoothing,
            MovingAverage
        };
        Q_ENUM(TransformateType)

        enum TriggersType {
            WithoutTriggers,
            TriggerByForwardFront,
            TriggerByBackFront,
            TriggerByTime
        };
        Q_ENUM(TriggersType)

        enum Colors {
            Black,
            Red,
            Blue,
            Green,
            Yellow
        };
        Q_ENUM(Colors)
        Q_GADGET

        Enums() = delete;
    };

}

#endif // ENUMS_H
