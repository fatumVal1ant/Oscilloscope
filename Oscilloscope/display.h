#ifndef DISPLAY_H
#define DISPLAY_H

#include <QList>
#include <complex>
#include <QtCharts/QtCharts>
QT_CHARTS_USE_NAMESPACE

#include "ichannel.h"
#include "marker.h"

namespace oscilloscope {
    class Display : public QChartView {
        Q_OBJECT

    private:
        QChart *_graph;
        QAreaSeries *_zoomArea;

        QList <Marker *> _dispalyMarkers;

        QLabel *_mouseCord;

        QLineSeries *_target;

        bool _mouseGrab;
        bool _targetMove;
        bool _zoomRect;

        bool _keyShift, _keyCtrl, _keyAlt;

        double _startX, _startY, _endX, _endY;

        std::complex<double> _expectedValue;
        std::complex<double> _standardDeviation;
        std::complex<double> _capacity;
        std::complex<double> _amplitude;

        double _complexAmplitude;

        QVector<QPointF *> _minAndMax;

        iChannel *_currentChannel;

        void verticalZoom(bool up);
        void horizontalZoom(bool up);
        void zoomRect(double minX, double maxX, double minY, double maxY);

        void createMarker(double x);
        void moveMarker(double x);

    protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);

        void wheelEvent(QWheelEvent *event);

        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);

    public:
        Display();

        void setCurrentChannel(iChannel *);

        void clear();
        void keysReset();

        void addGraph(QAbstractSeries *series);
        void deleteGraph(const QString &name);
        void deleteDublicatesGraph(const QString &name);

        void markersRecount();

        std::complex<double> expectedValue() const;
        std::complex<double> standardDeviation() const;
        std::complex<double> capacity() const;
        std::complex<double> amplitude() const;

        double complexAmplitude() const;

        QVector<QPointF *> minAndMax() const;

       ~Display();

    signals:
        void markersRecounted();

    public slots:
        void resetMarkers();
        void scaleByCenter();

    };
}

#endif // DISPLAY_H
