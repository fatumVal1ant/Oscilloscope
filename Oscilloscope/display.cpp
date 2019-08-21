#include "display.h"
#include "dublicatechannel.h"
#include "markermeasurements.h"

#include <QDebug>

namespace oscilloscope {
    /// КОНСТРУКТОР

    Display::Display() : QChartView() {
        _graph = new QChart();
        _graph->legend()->hide();

        this->setChart(_graph);

        this->setRenderHint(QPainter::Antialiasing);
        this->setStyleSheet("background-color: rgb(64, 64, 64)");

        // НАСТРОЙКА ОСЕЙ

        QValueAxis *xAxis = new QValueAxis();
        xAxis->setRange(0, 100);

        QValueAxis *yAxis = new QValueAxis();
        yAxis->setRange(-10, 10);

        _graph->addAxis(xAxis, Qt::AlignBottom);
        _graph->addAxis(yAxis, Qt::AlignLeft);

        // НАСТРОЙКА КЛАВИШ

        _mouseGrab = false;
        _zoomRect = false;

        _keyAlt = false;
        _keyShift = false;
        _keyCtrl = false;

        // СОЗДАНИЕ СЕРИИ, ОТВЕЧАЮЩАЯ ЗА ПРЯМОУГОЛЬНИК, ПО КОТОРОМУ БУДЕТ ПРОИСХОДИТЬ СКЕЙЛИНГ

        _zoomArea = new QAreaSeries();

        _graph->addSeries(_zoomArea);

        _zoomArea->attachAxis(_graph->axisX());
        _zoomArea->attachAxis(_graph->axisY());

        _zoomArea->hide();

        _zoomArea->setOpacity(0.2);
        _zoomArea->setBorderColor(Qt::black);
        _zoomArea->setBrush(QBrush(Qt::gray));

        // СОЗДАНИЕ ЛЭЙБЛА С КООРДИНАТАМИ КУРСОРА МЫШИ ОТНОСИТЕЛЬНО СИСТЕМЫ КООРДИНАТ

        this->setMouseTracking(true);
        _graph->setAcceptHoverEvents(true);

        _mouseCord = new QLabel(this);

        _mouseCord->setStyleSheet("color: black");
        _mouseCord->setAttribute(Qt::WA_TranslucentBackground);

        QFont font = _mouseCord->font();
        font.setPointSizeF(8);

        _mouseCord->setFont(font);

        _mouseCord->show();

        // СОЗДАНИЕ МАРКЕРОВ

        createMarker(_graph->plotArea().x());
        createMarker(_graph->plotArea().width() + _graph->plotArea().x());

        _currentChannel = nullptr;
    }

    /// ИЗМЕНЕНИЕ ТЕКУЩЕГО ВЫБРАННОГО КАНАЛА

    void Display::setCurrentChannel(iChannel *channel) {
        _currentChannel = channel;
    }

    /// ОБРАБОТКА СОБЫТИЙ НАЖАТИЯ КНОПОК МЫШИ

    void Display::mousePressEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            if (_keyCtrl) {
                _startX = event->globalX();
                _startY = event->globalY();

                _mouseGrab = true;
            } else {
                _startX = event->x();
                _startY = event->y();

                if (_startX >= _graph->plotArea().x() && _startX <= _graph->plotArea().x() + _graph->plotArea().width() &&
                        _startY >= _graph->plotArea().y() && _startY <= _graph->plotArea().y() + _graph->plotArea().height()) {

                    _startY = _graph->mapToValue(QPointF(_startX, _startY)).y();
                    _startX = _graph->mapToValue(QPointF(_startX, _startY)).x();

                    _endX = _startX;
                    _endY = _startY;

                    _zoomRect = true;
                }
            }
        } else if (event->button() == Qt::MiddleButton) {
            scaleByCenter();
        } else if (event->button() == Qt::RightButton) {
            _startX = event->x();
            _startY = event->y();

            if (_startX >= _graph->plotArea().x() && _startX <= _graph->plotArea().x() + _graph->plotArea().width() &&
                    _startY >= _graph->plotArea().y() && _startY <= _graph->plotArea().y() + _graph->plotArea().height())
                moveMarker(_startX);
        }
    }

    /// ОБРАБОТКА СОБЫТИЙ ОТПУСКАНИЯ КНОПОК МЫШИ

    void Display::mouseReleaseEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            _mouseGrab = false;

            if (_zoomRect) {
                _zoomArea->hide();

                if (_endX != _startX && _endY != _startY)
                    zoomRect(_startX, _endX, _startY, _endY);

                _zoomRect = false;
            }
        }
    }

    /// ОБРАБОТКА ДВИЖЕНИЯ КУРСОРА МЫШИ

    void Display::mouseMoveEvent(QMouseEvent *event) {
        // ОБНОВЛЕНИЕ ПОСЛЕДНИХ КООРДИНАТ

        double x = event->x();
        double y = event->y();

        if (x < _graph->plotArea().x() || x > _graph->plotArea().x() + _graph->plotArea().width()
                || y < _graph->plotArea().y() || y > _graph->plotArea().y() + _graph->plotArea().height()) {
            if (x < _graph->plotArea().x())
                x = _graph->plotArea().x();

            if (x > _graph->plotArea().x() + _graph->plotArea().width())
                x = _graph->plotArea().x() + _graph->plotArea().width();

            if (y < _graph->plotArea().y())
                y = _graph->plotArea().y();

            if (y > _graph->plotArea().y() + _graph->plotArea().height())
                y = _graph->plotArea().y() + _graph->plotArea().height();

            _mouseCord->hide();

            _endY = _graph->mapToValue(QPointF(x, y)).y();
            _endX = _graph->mapToValue(QPointF(x, y)).x();

        } else {
            _endY = _graph->mapToValue(QPointF(x, y)).y();
            _endX = _graph->mapToValue(QPointF(x, y)).x();

            _mouseCord->setText(QString::number(_endX, 'f', 2) + " : " + QString::number(_endY, 'f', 2));
            _mouseCord->setGeometry(QRect(x, y + 20, 100, 13));

            _mouseCord->show();
        }

        if (_mouseGrab) {
            // ПЕРЕДВИЖЕНИЕ ГРАФИКА

            double divX = (_startX - event->globalX());
            double divY = (event->globalY() - _startY);

            QLogValueAxis *rangeX = static_cast<QLogValueAxis *>(_graph->axisX());
            QLogValueAxis *rangeY = static_cast<QLogValueAxis *>(_graph->axisY());

            double x1 = rangeX->min();
            double x2 = rangeX->max();

            double stepX = abs(x2 - x1) / 1000 * (_keyShift ? 10 : 1);

            double y1 = rangeY->min();
            double y2 = rangeY->max();

            double stepY = abs(y2 - y1) / 1000 * (_keyShift ? 10 : 1);

            x1 += divX * stepX;
            x2 += divX * stepX;

            y1 += divY * stepY;
            y2 += divY * stepY;

            _graph->axisX()->setRange(x1, x2);
            _graph->axisY()->setRange(y1, y2);

            _startX = event->globalX();
            _startY = event->globalY();

            _dispalyMarkers.at(0)->update();
            _dispalyMarkers.at(1)->update();

            markersRecount();
        } else if (_zoomRect) {
            // ПРИБЛИЖЕНИЕ ГРАФИКА ПО ВЫДЕЛЕННОМУ ПРЯМОУГОЛЬНИКУ

            if (_startX != _endX && _startY != _endY) {
                QLineSeries *upper = new QLineSeries();
                QLineSeries *lower = new QLineSeries();

                upper->append(_startX, _startY);
                upper->append(_endX, _startY);

                lower->append(_startX, _endY);
                lower->append(_endX, _endY);

                _zoomArea->setUpperSeries(upper);
                _zoomArea->setLowerSeries(lower);

                _zoomArea->show();
            }
        }
    }

    /// ОБРАБОТКА СОБЫТИЙ КОЛЕСИКА МЫШИ

    void Display::wheelEvent(QWheelEvent *event) {
        if (event->angleDelta().y() > 0.5) {
            // ОТДАЛЕНИЕ

            if (!_keyAlt && !_keyCtrl) {
                horizontalZoom(true);
                verticalZoom(true);
            } else {
                if (_keyCtrl) horizontalZoom(true);
                    else verticalZoom(true);
            }
        } else if (event->angleDelta().y() < -0.5) {
            // ПРИБЛИЖЕНИЕ

            if (!_keyAlt && !_keyCtrl) {
                horizontalZoom(false);
                verticalZoom(false);
            } else {
                if (_keyCtrl) horizontalZoom(false);
                    else verticalZoom(false);
            }
        }
    } 

    /// ГОРИЗОНТАЛЬНОЕ ЗУМИРОВАНИЕ

    void Display::horizontalZoom(bool up) {
        if (up) {
            // ОТДАЛЕНИЕ

            QLogValueAxis *rangeX = static_cast<QLogValueAxis *>(_graph->axisX());

            double maxX = rangeX->max();
            double minX = rangeX->min();

            double step = abs(maxX - minX) / 100;

            _graph->axisX()->setMin(minX - step);
            _graph->axisX()->setMax(maxX + step);
        } else {
            // ПРИБЛИЖЕНИЕ

            QLogValueAxis *rangeX = static_cast<QLogValueAxis *>(_graph->axisX());

            double maxX = rangeX->max();
            double minX = rangeX->min();

            double step = abs(maxX - minX) / 100;

            _graph->axisX()->setMin(minX + step);
            _graph->axisX()->setMax(maxX - step);
        }

        _dispalyMarkers.at(0)->update();
        _dispalyMarkers.at(1)->update();

        markersRecount();
    }

    /// ВЕРТИКАЛЬНОЕ ЗУМИРОВАНИЕ

    void Display::verticalZoom(bool up) {
        if (up) {
            // ОТДАЛЕНИЕ

            QLogValueAxis *rangeY = static_cast<QLogValueAxis *>(_graph->axisY());

            double maxY = rangeY->max();
            double minY = rangeY->min();

            double step = abs(maxY - minY) / 100;

            _graph->axisY()->setMin(minY - step);
            _graph->axisY()->setMax(maxY + step);
        } else {
            // ПРИБЛИЖЕНИЕ

            QLogValueAxis *rangeY = static_cast<QLogValueAxis *>(_graph->axisY());

            double maxY = rangeY->max();
            double minY = rangeY->min();

            double step = abs(maxY - minY) / 100;

            _graph->axisY()->setMin(minY + step);
            _graph->axisY()->setMax(maxY - step);   
        }

        _dispalyMarkers.at(0)->update();
        _dispalyMarkers.at(1)->update();

        markersRecount();
    }

    /// ПРИБЛИЖЕНИЕ ПО ОБОЗНАЧЕННОМУ ПРЯМОУГОЛЬНИКУ

    void Display::zoomRect(double minX, double maxX, double minY, double maxY) {
        if (maxX > minX) _graph->axisX()->setRange(minX, maxX);
            else _graph->axisX()->setRange(maxX, minX);

        if (maxY > minY) _graph->axisY()->setRange(minY, maxY);
            else _graph->axisY()->setRange(maxY, minY);

        _dispalyMarkers.at(0)->update();
        _dispalyMarkers.at(1)->update();

        markersRecount();
    }

    /// ОЧИСТКА ГРАФИКА

    void Display::clear() {
        QList<QAbstractSeries *> list = _graph->series();

        for (int i = 3; i < list.size(); i++)
            _graph->removeSeries(list.at(i));
    }

    /// ДОБАВЛЕНИЕ КАНАЛА

    void Display::addGraph(QAbstractSeries *series) {
        deleteGraph(series->name());

        _graph->addSeries(series);

        series->attachAxis(_graph->axisX());
        series->attachAxis(_graph->axisY());
    }

    /// УДАЛЕНИЕ КАНАЛА

    void Display::deleteGraph(const QString &name) {
        QList<QAbstractSeries *> list = _graph->series();

        for (int i = 3; i < list.size(); i++)
            if (list.at(i)->name() == name)
                _graph->removeSeries(list.at(i));
    }

    /// УДАЛЕНИЕ ДУБЛИКАТОВ ПО НАЗВАНИЮ ОРИГИНАЛА

    void Display::deleteDublicatesGraph(const QString &name) {
        QList<QAbstractSeries *> list = _graph->series();

        for (int i = 3; i < list.size(); i++)
            if (list.at(i)->name().contains(DUBLICATE_NAME_BY_PARENT(name), Qt::CaseInsensitive) ||
                list.at(i)->name().contains(DUBLICATE_NAME_BY_PARENT(name) + "IMAG", Qt::CaseInsensitive))
                    _graph->removeSeries(list.at(i));
    }

    /// ОБРАБОТКА СОБЫТИЙ НАЖАТИЯ КЛАВИШ

    void Display::keyPressEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Alt)
            _keyAlt = true;

        else if (event->key() == Qt::Key_Control)
            _keyCtrl = true;

        else if (event->key() == Qt::Key_Shift)
            _keyShift = true;

        else if (event->key() == Qt::Key_Escape) {
            _zoomRect = false;
            _zoomArea->hide();
        }
    }

    /// ОБРАБОТКА СОБЫТИЙ ОТПУСКАНИЯ КЛАВИШ

    void Display::keyReleaseEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Alt)
            _keyAlt = false;

        else if (event->key() == Qt::Key_Control) {
            _keyCtrl = false;
            _mouseGrab = false;
        } else if (event->key() == Qt::Key_Shift)
            _keyShift = false;
    }

    /// СБРОС НАЖАТИЯ КЛАВИШ

    void Display::keysReset() {
        this->_keyAlt = false;
        this->_keyCtrl = false;
        this->_keyShift = false;

        this->_mouseGrab = false;
        this->_zoomRect = false;

        this->_zoomArea->hide();
        this->_mouseCord->hide();
    }

    /// СОЗДАНИЕ МАРКЕРОВ

    void Display::createMarker(double x) {
        Marker *marker = new Marker(_graph, x);
        _dispalyMarkers << marker;
    }

    /// ПЕРЕДВИЖЕНИЕ МАРКЕРОВ

    void Display::moveMarker(double x) {
        _dispalyMarkers[0]->setAnchor(x);

        Marker *marker = _dispalyMarkers[0];

        _dispalyMarkers[0] = _dispalyMarkers[1];
        _dispalyMarkers[1] = marker;

        markersRecount();
    }

    /// ПЕРЕРАСЧЕТ МАРКЕРОВ

    void Display::markersRecount() {
        if (_currentChannel) {
            QVector<double> x = _currentChannel->data()->frame()->_offsetX;
            QVector<std::complex<double>> y = _currentChannel->points();

            _expectedValue =  MarkerMeasurements::expectedValue(x, y, _dispalyMarkers);
            _standardDeviation =  MarkerMeasurements::standardDeviation(x, y, _dispalyMarkers, _expectedValue);
            _capacity = MarkerMeasurements::signalCapacity(x, y, _dispalyMarkers);
            _minAndMax = MarkerMeasurements::minAndMax(x, y, _dispalyMarkers);
            _amplitude = MarkerMeasurements::signalAmplitude(_minAndMax);
            _complexAmplitude = MarkerMeasurements::complexAmplitude(x, y, _dispalyMarkers);

            emit markersRecounted();
        }
    }

    /// ГЕТТЕР МАТ ОЖИДАНИЯ

    std::complex<double> Display::expectedValue() const {
        return _expectedValue;
    }

    /// ГЕТТЕР СКО

    std::complex<double> Display::standardDeviation() const {
        return _standardDeviation;
    }

    /// ГЕТТЕР МОЩНОСТИ

    std::complex<double> Display::capacity() const {
        return _capacity;
    }

    /// ГЕТТЕР АМПЛИТУДЫ

    std::complex<double> Display::amplitude() const {
        return _amplitude;
    }

    /// ГЕТТЕР КОМПЛЕКСНОЙ АМПЛИТУДЫ

    double Display::complexAmplitude() const {
        return _complexAmplitude;
    }

    /// ГЕТТЕР ТОЧЕК ЭКСТРЕМУМА

    QVector<QPointF *> Display::minAndMax() const {
        return _minAndMax;
    }

    /// СБРОС МАРКЕРОВ

    void Display::resetMarkers() {
        moveMarker(_graph->plotArea().x());
        moveMarker(_graph->plotArea().width() + _graph->plotArea().x());

        _dispalyMarkers.at(0)->hide();
        _dispalyMarkers.at(1)->hide();
    }

    /// СКЕЙЛИНГ ПО ЦЕНТРУ

    void Display::scaleByCenter() {
        QList<QAbstractSeries *> list = _graph->series();

        if (list.size() > 3) {
            QLineSeries *line = static_cast<QLineSeries *>(list.at(3));
            QVector<QPointF> points = line->pointsVector();

            double minX = points.at(0).x(), maxX = points.at(0).x(), maxY = points.at(0).y(), minY = points.at(0).y();

            for (int i = 3; i < list.size(); i++) {
                line = static_cast<QLineSeries *>(list.at(i));
                points = line->pointsVector();

                for (int j = 0; j < points.size(); j++) {
                    double x = points.at(j).x();
                    double y = points.at(j).y();

                    if (x > maxX) maxX = x;
                    if (x < minX) minX = x;

                    if (y > maxY) maxY = y;
                    if (y < minY) minY = y;
                }
            }

            zoomRect(minX, maxX, minY < 0 ? minY * 2 : minY / 2, maxY > 0 ? maxY * 2 : maxY / 2);
        } else zoomRect(0, 100, -10, 10);
    }

    /// ДЕСТРУКТОР

    Display::~Display() {}
}
