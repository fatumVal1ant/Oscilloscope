#include <QDropEvent>
#include <QMimeData>
#include <QDrag>

#include <QListWidgetItem>

#include "localchannellistview.h"
#include "dublicatechannel.h"

#include <QDebug>

namespace oscilloscope {
    /// СОЗДАНИЕ ЛОКАЛЬНОГО СПИСКА ОТОБРАЖЕНИЯ КАНАЛОВ И ИХ ДУБЛИКАТОВ

    LocalChannelListView::LocalChannelListView(QWidget* parent) : iListView(parent) {
        setFixedWidth(150);
        setAcceptDrops(true);
    }

    /// ОТСЛЕЖИВАНИЯ ПЕРЕМЕЩЕНИЯ ЗАХВАЧЕННОГО ОБЪЕКТА

    void LocalChannelListView::dragMoveEvent(QDragMoveEvent *event) {
        if (event->mimeData()->hasFormat("application/x-item") && event->source() != this) {
            QString name = event->mimeData()->data("application/x-item");

            if (!itemByName(name)) event->accept();
                else event->ignore();
        } else event->ignore();
    }

    /// ПРИНЯТИЕ ЗАХВАЧЕННОГО ОБЪЕКТА

    void LocalChannelListView::dropEvent(QDropEvent *event) {
        if (event->mimeData()->hasFormat("application/x-item")) {
            event->accept();

            QString name = event->mimeData()->data("application/x-item");

            addChannel(name);
        } else event->ignore();
    }

    /// ОТСЛЕЖИВАНИЯ СТОЛКНОВЕНИЯ С ЗАХВАЧЕННЫМ ОБЪЕКТОМ

    void LocalChannelListView::dragEnterEvent(QDragEnterEvent *event) {
        if (event->mimeData()->hasFormat("application/x-item")) event->accept();
            else event->ignore();
    }

    /// ПОЛУЧЕНИЕ ОРИГИНАЛА ПО ЕГО ИМЕНИ

    QListWidgetItem *LocalChannelListView::itemByName(const QString &name) const {
        QCheckBox *item;

        for (int i = 0; i < this->count(); i++) {
            item = dynamic_cast<QCheckBox *>(this->itemWidget(this->item(i)));

            if (item->text() == name)
                return this->item(i);
        }

        return 0;
    }

    /// ПОЛУЧЕНИЕ ДУБЛИКАТА ПО ЕГО ИМЕНИ

    QListWidgetItem *LocalChannelListView::itemDublicateByName(const QString &name) const {
        QCheckBox *item;

        for (int i = 0; i < this->count(); i++) {
            item = dynamic_cast<QCheckBox *>(this->itemWidget(this->item(i)));
            if (item->text().contains(DUBLICATE_NAME_BY_PARENT(name), Qt::CaseInsensitive))
                return this->item(i);
        }

        return 0;
    }

    /// ДОБАВЛЕНИЯ КАНАЛА В СПИСОК ОТОБРАЖЕНИЯ

    void LocalChannelListView::addChannel(const QString &name) {;
        QCheckBox *channel = new QCheckBox(name);
        connect(channel, SIGNAL(stateChanged(int)), this, SLOT(itemCheck()));

        QListWidgetItem *item = new QListWidgetItem(this);

        setItemWidget(item, channel);

        Attributes *atr = new Attributes();
        _attributes.append(atr);
    }

    /// ОБРАБОТКА СИГНАЛА О ИЗМЕНЕНИИ СОСТОЯНИЯ CHECKBOX`а

    void LocalChannelListView::itemCheck() {
        emit itemChecked(sender());
    }

    /// УДАЛЕНИЕ ОБЪЕКТА ИЗ СПИСКА ОТОБРАЖЕНИЯ

    void LocalChannelListView::itemDelete(QListWidgetItem *item) {
        QCheckBox *channel = dynamic_cast<QCheckBox *>(itemWidget(item));

        emit channelDeleted(channel->text());

        deleteAttribute(this->row(item));
        delete this->takeItem(this->row(item));
    }

    /// УДАЛЕНИЕ ОРИГИНАЛА ПО ЕГО НАЗВАНИЮ

    void LocalChannelListView::deleteChannel(const QString &name) {
        QListWidgetItem *item = this->itemByName(name);

        if (item) {
            deleteAttribute(this->row(item));
            if (item) delete item;

            emit channelDeleted(name);
        }
    }

    /// УДАЛЕНИЕ ВСЕХ ДУБЛИКАТОВ ПО НАЗВАНИЮ ОРИГИНАЛА

    void LocalChannelListView::deleteDublicates(const QString &name) {
        QListWidgetItem *item;

        while ((item = this->itemDublicateByName(name))) {
            deleteAttribute(this->row(item));
            delete item;
        }
    }

    /// ПОЛУЧЕНИЕ АТРИБУТОВ ОТОБРАЖЕНИЯ КОНКРЕТНОГО КАНАЛА

    Attributes *LocalChannelListView::attribute(int index) const {
        return _attributes.at(index);
    }

    /// УДАЛЕНИЕ АТРИБУТА ИЗ СПИСКА

    void LocalChannelListView::deleteAttribute(int index) {
        delete _attributes.at(index);
        _attributes.removeAt(index);
    }

    /// ДЕСТРУКТОР

    LocalChannelListView::~LocalChannelListView() {
        for (int i = 0; i < _attributes.length(); i++)
            delete _attributes.at(i);
    }
}
