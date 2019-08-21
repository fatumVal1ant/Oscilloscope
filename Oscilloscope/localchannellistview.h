///     ВИДЖЕТ СПИСКА КАНАЛОВ ОДНОГО SIMPLESCOP`а С ДУБЛИКАТАМИ

#ifndef LOCALCHANNELLISTVIEW_H
#define LOCALCHANNELLISTVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QList>
#include <QCheckBox>

#include "ilistview.h"
#include "attributes.h"

namespace Ui {
    class localChannelListView;
}

namespace oscilloscope {
    class LocalChannelListView : public iListView {
        Q_OBJECT

    public:
        explicit LocalChannelListView(QWidget *parent = 0);

        Attributes *attribute(int index) const;
        QListWidgetItem *itemByName(const QString &name) const;
        QListWidgetItem *itemDublicateByName(const QString &name) const;

        void addChannel(const QString &name);
        void deleteChannel(const QString &name);
        void deleteDublicates(const QString &name);
        void deleteAttribute(int index);

        ~LocalChannelListView();

    protected:
        void dragMoveEvent(QDragMoveEvent *event);
        void dropEvent(QDropEvent *event);
        void dragEnterEvent(QDragEnterEvent *event);

        void itemDelete(QListWidgetItem *item);

    private:
        QList<Attributes *> _attributes;

    private slots:
        void itemCheck();

    signals:
        void itemChecked(QObject *);
    };
}

#endif
