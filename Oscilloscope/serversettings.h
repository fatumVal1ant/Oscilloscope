#ifndef SERVERSETTINGSDIALOG_H
#define SERVERSETTINGSDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QIntValidator>
#include <QPushButton>

namespace oscilloscope {
    class ServerSettings : public QDialog {
        Q_OBJECT

    public:
        explicit ServerSettings(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    private:
        QGridLayout *_layout;
        QHBoxLayout *_buttonLayout;
        QLineEdit *_udpLine;
        QLineEdit *_tcpLine;
        QLabel *_udpLabel;
        QLabel *_tcpLabel;
        QPushButton *_okButton;
        QPushButton *_cancelButton;
        QIntValidator *_portValidator;

        quint16 _udpPort;
        quint16 _tcpPort;

        private slots:
            void clickOk();
            void clickCancel();

        signals:
            void tcpPortChanged();
            void udpPortChanged();
    };
}
#endif // SERVERSETTINGSDIALOG_H
