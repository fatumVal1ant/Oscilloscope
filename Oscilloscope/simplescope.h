///     SIMPLESCOPE, СОЗДАЕТ ДИСПЛЕЙ ДЛЯ ОТРИСОВКИ ГРАФИКОВ И СПИСОК СВОИХ КАНАЛОВ

#ifndef SIMPLESCOPE_H
#define SIMPLESCOPE_H

#include <QWidget>
#include <QMetaEnum>
#include <tuple>

#include <QtCharts/QtCharts>
QT_CHARTS_USE_NAMESPACE

#include "localchannellist.h"
#include "globalchannellist.h"
#include "display.h"

#define SIMPLESCOPE_NAME_DEF "Неопределенный дисплей"

namespace Ui {
    class SimpleScope;
}

namespace oscilloscope {
    class SimpleScope : public QWidget {
        Q_OBJECT

    public:
        explicit SimpleScope(QWidget *parent = 0, const QString &name = SIMPLESCOPE_NAME_DEF, GlobalChannelList *globalList = 0);

        LocalChannelList *localList() const;

        int colorIndex(const QColor &color) const;
        const QColor colorByIndex(int index) const;
        bool pause() const;

        ~SimpleScope();

    private:
        Ui::SimpleScope *ui;

        GlobalChannelList *_globalList;
        LocalChannelList *_channels;
        bool _stop;

        Display *_display;

        QAction *_pauseAct;

        std::tuple<iChannel *, Attributes *, bool> findChannel(const QString &name);

        QMenuBar *_bar;

    protected:
        void paintEvent(QPaintEvent *);
        bool eventFilter(QObject *object, QEvent *event);

    private slots:
        void on_createDublicate_pressed();
        void updateTitle();
        void setPause();

    public slots:
        void channelUpdate(std::tuple<iChannel *, Attributes *, bool> _tuple, QString name);

        void itemChecked(QObject *);

        void recount(const QString &name);
        void recountDublicates(const QString &name);

        void deleteChannel(const QString &name);
        void deleteDublicates(const QString &name);

        void markersRecounted();
        void minMaxUpdate();
    };
}

#endif
