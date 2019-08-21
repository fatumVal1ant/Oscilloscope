#include "marker.h"

namespace oscilloscope {
    /// КОНСТРУКТОР МАРКЕРА

    Marker::Marker(QChart *graph, double x) {
        _graph = graph;
        _x = x;

        QLogValueAxis *rangeY = static_cast<QLogValueAxis*>(_graph->axisY());

        _series = new QLineSeries();
        _series->setName("Marker");

        _series->append(_graph->mapToValue(QPointF(_x, 0)).x(), rangeY->min());
        _series->append(_graph->mapToValue(QPointF(_x, 0)).x(), rangeY->max());

        _graph->addSeries(_series);

        _series->attachAxis(_graph->axisX());
        _series->attachAxis(_graph->axisY());

        QPen pen(QColor(255, 0, 0, 100));
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);

        _series->setPen(pen);

        _series->hide();
    }

    /// ГЕТТЕР ПОЛОЖЕНИЯ МАРКЕРА ВНЕ СИСТЕМЫ КООРДИНАТ

    double Marker::anchor() const {
        return _x;
    }

    /// ИЗМЕНЕНИЕ ПОЛОЖЕНИЯ МАРКЕРА ПО КООРДИНАТЕ ВНЕ СИСТЕМЫ КООРДИНАТ

    void Marker::setAnchor(double x) {
        _x = x;

        update();

        _series->show();
    }

    /// СКРЫТИЕ МАРКЕРА

    void Marker::hide() {
        _series->hide();
    }

    /// ГЕТТЕР ПОЛОЖЕНИЯ МАРКЕРА В СИСТЕМЕ КООРДИНАТ

    double Marker::localX() const {
        return _graph->mapToValue(QPointF(_x, 0)).x();
    }

    /// ОБНОВЛЕНИЕ ПОЛОЖЕНИЯ МАРКЕРА

    void Marker::update() {
        QLogValueAxis *rangeY = static_cast<QLogValueAxis*>(_graph->axisY());

        _series->replace(0, _graph->mapToValue(QPointF(_x, 0)).x(), rangeY->min());
        _series->replace(1, _graph->mapToValue(QPointF(_x, 0)).x(), rangeY->max());
    }

    /// ДЕСТРУКТОР

    Marker::~Marker() {}
}
