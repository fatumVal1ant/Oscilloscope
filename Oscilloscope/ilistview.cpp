#include <QKeyEvent>

#include "ilistview.h"

namespace oscilloscope {
    /// СОЗДАНИЕ СПИСКА ОТОБРАЖЕНИЯ КАНАЛОВ

    iListView::iListView(QWidget* parent) : QListWidget(parent) {
        this->setSelectionMode(QAbstractItemView::SingleSelection);
        this->setDropIndicatorShown(true);

        setStyleSheet("color: Black;");
    }

    /// ОБРАБОТКА НАЖАТИЙ КЛАВИШ

    void iListView::keyPressEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Backspace && this->currentRow() >= 0)
            itemDelete(this->currentItem());
    }


    /// ДЕСТРУКТОР

    iListView::~iListView() {}
}
