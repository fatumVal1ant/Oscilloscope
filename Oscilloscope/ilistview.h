///     РОДИТЕЛЬ LISTVIEW

#ifndef ILISTVIEW_H
#define ILISTVIEW_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
    class iListView;
}

namespace oscilloscope {
    class iListView : public QListWidget {
        Q_OBJECT

    public:
        explicit iListView(QWidget *parent = 0);

        ~iListView();

    protected:
        void keyPressEvent(QKeyEvent *event);
        virtual void itemDelete(QListWidgetItem *item) = 0;

    signals:
        void channelDeleted(const QString &name);
    };
}

#endif
