#ifndef MARKER_H
#define MARKER_H

#include <QtCharts/QtCharts>

namespace oscilloscope {
    class Marker {
    public:
        Marker(QChart *graph, double x);
        ~Marker();

        double anchor() const;
        void setAnchor(double x);
        double localX() const;

        void update();
        void hide();

    private:
        QChart *_graph;
        QLineSeries *_series;

        double _x;
    };
}

#endif // MARKER_H
