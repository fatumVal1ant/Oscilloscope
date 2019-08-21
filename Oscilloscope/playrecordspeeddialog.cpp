#include "playrecordspeeddialog.h"

namespace oscilloscope {
    PlayRecordSpeedDialog::PlayRecordSpeedDialog(RecordFrameParser* parser, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), _parser(parser) {
        setWindowTitle("Скорость воспроизведения");

        _milliPeriodLabel = new QLabel("Миллисекунды", this);

        _validator = new QIntValidator(this);
        _validator->setBottom(0);
        _milliPeriodEdit = new QLineEdit(QString::number(_parser->milliPeriod()), this);
        _milliPeriodEdit->setValidator(_validator);

        _okButton = new QPushButton("Ок", this);
        connect(_okButton, SIGNAL(clicked()), this, SLOT(clickOk()));

        _cancelButton = new QPushButton("Отмена", this);
        connect(_cancelButton, SIGNAL(clicked()), this, SLOT(clickCancel()));

        _strongRightButton = new QPushButton(">>", this);
        connect(_strongRightButton, SIGNAL(clicked()), this, SLOT(clickStrongRight()));
        _weakRightButton = new QPushButton(">", this);
        connect(_weakRightButton, SIGNAL(clicked()), this, SLOT(clickWeakRight()));

        _strongLeftButton = new QPushButton("<<", this);
        connect(_strongLeftButton, SIGNAL(clicked()), this, SLOT(clickStrongLeft()));
        _weakLeftButton = new QPushButton("<", this);
        connect(_weakLeftButton, SIGNAL(clicked()), this, SLOT(clickWeakLeft()));

        _layout = new QGridLayout(this);
        setLayout(_layout);
        _layout->addWidget(_milliPeriodLabel, 0, 0);
        _layout->addWidget(_milliPeriodEdit, 0, 1);

        _speedButtonLayout = new QHBoxLayout();
        _layout->addLayout(_speedButtonLayout, 1, 0, 1, 2);
        _speedButtonLayout->addWidget(_strongLeftButton);
        _speedButtonLayout->addWidget(_weakLeftButton);
        _speedButtonLayout->addWidget(_weakRightButton);
        _speedButtonLayout->addWidget(_strongRightButton);

        _mainButtonLayout = new QHBoxLayout();
        _layout->addLayout(_mainButtonLayout, 2, 0, 1, 2);
        _mainButtonLayout->addWidget(_okButton);
        _mainButtonLayout->addWidget(_cancelButton);

        this->setStyleSheet("background-color: rgb(64, 64, 64);"
                            "color: white;");

        setWindowFlags(Qt::Dialog | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    }

    void PlayRecordSpeedDialog::clickOk() {
        int milli = _milliPeriodEdit->text().toInt();
        _parser->setMilliPeriod(milli);
        this->close();
    }

    void PlayRecordSpeedDialog::clickCancel() {
        this->close();
    }

    void PlayRecordSpeedDialog::clickStrongRight() {
        changeSpeed(1000);
    }

    void PlayRecordSpeedDialog::clickWeakRight() {
        changeSpeed(100);
    }

    void PlayRecordSpeedDialog::clickStrongLeft() {
        changeSpeed(-1000);
    }

    void PlayRecordSpeedDialog::clickWeakLeft() {
        changeSpeed(-100);
    }

    void PlayRecordSpeedDialog::changeSpeed(int milliDelta) {
        int currentSpeed = _milliPeriodEdit->text().toInt();

        int newSpeed = currentSpeed + milliDelta;
        if (newSpeed <= 0) return;

        _milliPeriodEdit->setText(QString::number(newSpeed));
    }
}
