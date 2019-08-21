#ifndef PLAYRECORDSPEEDDIALOG_H
#define PLAYRECORDSPEEDDIALOG_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "recordframeparser.h"

namespace oscilloscope {
    class PlayRecordSpeedDialog : public QDialog {
        Q_OBJECT

    private:
        RecordFrameParser *_parser;
        QLabel *_milliPeriodLabel;
        QLineEdit *_milliPeriodEdit;
        QIntValidator *_validator;
        QGridLayout *_layout;
        QHBoxLayout *_mainButtonLayout;
        QHBoxLayout *_speedButtonLayout;

        QPushButton *_okButton;
        QPushButton *_cancelButton;
        QPushButton *_strongRightButton;
        QPushButton *_weakRightButton;
        QPushButton *_strongLeftButton;
        QPushButton *_weakLeftButton;

        void changeSpeed(int milliDelta);

    private slots:
        void clickOk();
        void clickCancel();
        void clickStrongRight();
        void clickWeakRight();
        void clickStrongLeft();
        void clickWeakLeft();

    public:
        PlayRecordSpeedDialog(RecordFrameParser *parser, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    };
}
#endif // PLAYRECORDSPEEDDIALOG_H
