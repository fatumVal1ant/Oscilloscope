#include <QDropEvent>
#include <QMimeData>
#include <QDrag>

#include "channellist.h"
#include "globalchannellistview.h"

#include <QDebug>

namespace oscilloscope {
    /// КОНСТРУТОР - СОЗДАНИЕ СПИСКА ОТОБРАЖЕНИЯ ОРИГИНАЛЬНЫХ КАНАЛОВ

    GlobalChannelListView::GlobalChannelListView(QWidget* parent) : iListView(parent) {
        this->setDragEnabled(true);
    }

    /// ЗАХВАТ ОБЪЕКТА

    void GlobalChannelListView::startDrag(Qt::DropActions) {
        QListWidgetItem *item = currentItem();
        QMimeData *mimeData = new QMimeData;

        QByteArray channel = item->text().toUtf8();
        mimeData->setText(channel);
        mimeData->setData("application/x-item", channel);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);

        drag->exec(Qt::CopyAction);
    }

    /// УДАЛЕНИЕ ОТКЛЮЧИВШЕГОСЯ КАНАЛА

    void GlobalChannelListView::itemDelete(QListWidgetItem *item) {
        if (item->foreground() == CH_DEAD) {
            emit channelDeleted(item->text());
            delete this->takeItem(this->row(item));
        }
    }

    /// ДЕСТРУКТОР

    GlobalChannelListView::~GlobalChannelListView() {}
}
