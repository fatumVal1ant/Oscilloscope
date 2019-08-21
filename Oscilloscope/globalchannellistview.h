///     ВИДЖЕТ ОТРИСОВКИ СПИСКА ОРИГИНАЛОВ КАНАЛОВ

#ifndef GLOBALCHANNELLISTVIEW_H
#define GLOBALCHANNELLISTVIEW_H

#include <QWidget>
#include <QListWidget>

#include "ilistview.h"

namespace Ui {
    class globalChannelListView;
}

namespace oscilloscope {
    class GlobalChannelListView : public iListView {
        Q_OBJECT

    public:
        explicit GlobalChannelListView(QWidget *parent = 0);

        ~GlobalChannelListView();

    protected:
        void startDrag(Qt::DropActions supportedActions);

        void itemDelete(QListWidgetItem *item);
    };
}

#endif
