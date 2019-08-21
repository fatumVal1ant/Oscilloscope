#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "localchannellistview.h"
#include "serversettings.h"
#include "playrecordspeeddialog.h"

namespace oscilloscope {
    /// СОЗДАНИЕ ГЛАВНОГО ОКНА И ПОРАЖДЕНИЕ ДРУГИХ

    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
        ui = new Ui::MainWindow;
        ui->setupUi(this);

        // СОЗДАНИЕ ОБЪЕКТОВ

        _channels = new GlobalChannelList(ui->verticalLayout);
        _channelController = new ChannelController(_channels);
        _recordItem = nullptr;

        this->show();
        this->move(this->x() - this->x() / 2, this->y());

        countScopes = 1;

        on_createSimpleScope_pressed();

        // СОЗДАНИЕ МЕНЮ БАРА

        QMenu *menu = menuBar()->addMenu("Файл");

        QAction *serverAct = new QAction("Параметры сервера", this);
        connect(serverAct, SIGNAL(triggered()), this, SLOT(changeServerSettings()));
        menu->addAction(serverAct);

        _recordMenu = menu->addMenu("Запись");

        _startRecordAct = new QAction("Начать запись", this);
        connect(_startRecordAct, SIGNAL(triggered()), this, SLOT(startRecord()));
        _startRecordAct->setEnabled(false);

        _stopRecordAct = new QAction("Остановить запись", this);
        connect(_stopRecordAct, SIGNAL(triggered()), this, SLOT(stopRecord()));
        _stopRecordAct->setEnabled(false);

        _loadAct = new QAction("Загрузить запись", this);
        connect(_loadAct, SIGNAL(triggered()), this, SLOT(loadRecord()));

        _playAct = new QAction("Воспроизвести");
        connect(_playAct, SIGNAL(triggered()), this, SLOT(playRecord()));
        _playAct->setEnabled(false);

        _stopAct = new QAction("Стоп");
        connect(_stopAct, SIGNAL(triggered()), this, SLOT(stopPlayingRecord()));
        _stopAct->setEnabled(false);

        _speedRecordMenu = new QAction("Скорость воспроизведения");
        connect(_speedRecordMenu, SIGNAL(triggered()), this, SLOT(changeRecordSpeedSettings()));
        _speedRecordMenu->setEnabled(false);

        _recordMenu->addAction(_startRecordAct);
        _recordMenu->addAction(_stopRecordAct);

        _recordMenu->addSeparator();

        _recordMenu->addAction(_loadAct);
        _recordMenu->addAction(_playAct);
        _recordMenu->addAction(_stopAct);
        _recordMenu->addAction(_speedRecordMenu);

        // СОЕДИНЕНИЕ СЛОТОВ И СИГНАЛОВ

        connect(_channels, SIGNAL(channelDeleted(QString)), this, SLOT(channelDelete(QString)));
        connect(_channels, SIGNAL(channelUpdated(QString)), this, SLOT(channelUpdate(QString)));
        connect(_channelController, SIGNAL(channelUpdated(QString)), this, SLOT(channelUpdate(QString)));
        connect(_channels, SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)), this, SLOT(updateRecordMenu()));

        this->installEventFilter(this);
    }

    /// НАЧАТЬ ЗАПИСЬ

    void MainWindow::startRecord() {
        iChannel *item = _channels->getSelectedChannel();
        if (item == nullptr) return;

        _recordItem = _channels->getSelectedItem();
        _recordItem->setIcon(QIcon(":/new/icons/record_icon"));

        _startRecordAct->setEnabled(false);
        _stopRecordAct->setEnabled(true);

        _recorder = new Recorder(item->data());
        connect(_recorder, SIGNAL(error(QString)), this, SLOT(showErrorMessage(QString)));
        _recorder->startRecord();
    }

    /// ОСТАНОВИТЬ ЗАПИСЬ

    void MainWindow::stopRecord() {
        if (_recordItem) _recordItem->setIcon(QIcon());
        _recordItem = nullptr;

        _startRecordAct->setEnabled(true);
        _stopRecordAct->setEnabled(false);

        _recorder->stopRecord();

        QString filename = QFileDialog::getSaveFileName(this, "Сохранение записи", QString(), "Record (*.rcd)");
        if (!filename.isEmpty()) _recorder->saveRecord(filename);

        delete _recorder;
        _recorder = nullptr;
    }

    /// ЗАГРУЗКА ЗАПИСИ

    void MainWindow::loadRecord() {
        QString filename = QFileDialog::getOpenFileName(this, "Открыть запись", QString(), "Record (*.rcd)");
        if (filename.isEmpty()) return;

        RecordFrameParser *recordFrameParser = new RecordFrameParser(filename, this);
        connect(recordFrameParser, SIGNAL(error(QString)), this, SLOT(showErrorMessage(QString)));

        if (!recordFrameParser->init()) {
            delete recordFrameParser;
            return;
        }

        _recordFrameParsers.push_back(recordFrameParser);
        _channelController->addRecordFrameParser(recordFrameParser);
    }

    /// ВОСПРОИЗВЕДЕНИЕ ЗАПИСИ

    void MainWindow::playRecord() {
        iChannel *item = _channels->getSelectedChannel();
        if (item == nullptr) return;

        QString channelName = item->data()->frame()->_channelName;

        for(int i = 0; i < _recordFrameParsers.size(); ++i) {
            if (_recordFrameParsers.at(i)->channelName() == channelName) {
                RecordFrameParser *parser =  _recordFrameParsers.at(i);

                parser->stop();
                parser->start();

                return;
            }
        }
    }

    /// ПАУЗА ВОСРОИЗВЕДЕННОЙ ЗАПИСИ

    void MainWindow::stopPlayingRecord() {
        iChannel *item = _channels->getSelectedChannel();
        if (item == nullptr) return;
        QString channelName = item->data()->frame()->_channelName;

        for(int i = 0; i < _recordFrameParsers.size(); ++i) {
            if (_recordFrameParsers.at(i)->channelName() == channelName) {
                RecordFrameParser* parser =  _recordFrameParsers.at(i);
                parser->stop();
                return;
            }
        }
    }

    /// СОЗДАНИЯ ОКНА С НАСТРОЙКАМИ ВОСПРОИЗВЕДЕНИЯ ЗАПИСИ

    void MainWindow::changeRecordSpeedSettings() {
        iChannel *item = _channels->getSelectedChannel();
        if (item == nullptr) return;

        QString channelName = item->data()->frame()->_channelName;

        for (int i = 0; i < _recordFrameParsers.size(); ++i) {
            if (_recordFrameParsers.at(i)->channelName() == channelName) {
                RecordFrameParser *parser =  _recordFrameParsers.at(i);
                PlayRecordSpeedDialog dialog(parser, this);
                dialog.exec();

                return;
            }
        }
    }

    /// ОБНОВЛЕНИЕ СПИСКА ДИСПЛЕЕВ ПРИ ЗАКРЫТИЕ ОДНОГО ИЗ НИХ

    void MainWindow::deleteScope() {
        QObject *scope = sender();

        for (int i = 0; i < _scopes.length(); i++)
            if (_scopes.at(i) == scope) {
                _scopes.removeAt(i);
                break;
            }
    }

    /// НАЖАТИЕ КНОПКИ - СОЗДАНИЕ НОВОГО ДИСПЛЕЯ

    void MainWindow::on_createSimpleScope_pressed() {
        SimpleScope *scope = new SimpleScope(0, "Дисплей " + QString::number(countScopes), _channels);
        scope->show();
        scope->move(this->x() + this->width() + (30 + 10 * _scopes.length()), this->y() + (10 * _scopes.length()));

        QObject::connect(scope, SIGNAL(destroyed()), this, SLOT(deleteScope()));

        _scopes.append(scope);
        countScopes++;
    }

    /// ОБРАБОТКА ОБНОВЛЕНИЯ КАНАЛОВ

    void MainWindow::channelUpdate(const QString &name) {
        for (int i = 0; i < _scopes.length(); i++) {
            if (!_scopes.at(i)->pause()) {
                _scopes.at(i)->repaint();

                _scopes.at(i)->recount(name);
                _scopes.at(i)->recountDublicates(name);
            }
        }
    }

    /// ОБРАБОТКА УДАЛЕНИЯ ОРИГИНАЛЬНОГО КАНАЛА

    void MainWindow::channelDelete(const QString &name) {
        if (_recordItem && _recordItem->text() == name) stopRecord();

        for (int i = 0; i < _recordFrameParsers.size(); i++) {
            if (_recordFrameParsers.at(i)->channelName() == name) {
                _recordFrameParsers.removeAt(i);
                break;
            }
        }

        for (int i = 0; i < _scopes.length(); i++) {
            SimpleScope *scope = _scopes.at(i);

            scope->localList()->channelsView()->deleteChannel(name);

            scope->deleteDublicates(name);

            scope->localList()->channelsView()->deleteDublicates(name);
            scope->localList()->dublicatesDelete(name);

            scope->deleteChannel(name);

            scope->repaint();
        }
    }

    /// ОБНОВЛЕНИЕ МЕНЮ ЗАПИСЕЙ

    void MainWindow::updateRecordMenu() {
        iChannel *item = _channels->getSelectedChannel();
        if (item == nullptr) {
            _startRecordAct->setEnabled(false);
            _stopRecordAct->setEnabled(false);
            _playAct->setEnabled(false);
            _stopAct->setEnabled(false);
            _speedRecordMenu->setEnabled(false);

            return;
        }

        QString channelName = item->data()->frame()->_channelName;

        for (int i = 0; i < _recordFrameParsers.size(); ++i) {
            if (_recordFrameParsers.at(i)->channelName() == channelName) {
                _startRecordAct->setEnabled(false);
                _playAct->setEnabled(true);
                _stopAct->setEnabled(true);
                _speedRecordMenu->setEnabled(true);

                return;
            }
        }

        if (!_recordItem) _startRecordAct->setEnabled(true);

        _playAct->setEnabled(false);
        _stopAct->setEnabled(false);
        _speedRecordMenu->setEnabled(false);
    }

    /// СОЗДАНИЕ ОКНА НАСТРОЕК СЕРВЕРА

    void MainWindow::changeServerSettings() {
          ServerSettings dialog(this);

          connect(&dialog, SIGNAL(tcpPortChanged()), _channelController, SLOT(reloadTcpServer()));
          connect(&dialog, SIGNAL(udpPortChanged()), _channelController, SLOT(reloadUdpServer()));

          dialog.exec();
    }

    /// ФИЛЬТР СОБЫТИЙ

    bool MainWindow::eventFilter(QObject *object, QEvent *event) {
        if (event->type() == QEvent::Close) {
            if (object == this) {
                for (int i = 0; i < _scopes.length(); i++)
                    delete _scopes.at(i--);
            }
        }

        return false;
    }

    /// ВЫВОД СООБЩЕНИЯ ОБ ОШИБКЕ

    void MainWindow::showErrorMessage(const QString &errorMessage) {
        QMessageBox errorMessageBox(this);

        errorMessageBox.setText(errorMessage);
        errorMessageBox.setWindowTitle("Ошибка");

        errorMessageBox.exec();
    }

    /// ДЕСТРУКТОР

    MainWindow::~MainWindow() {
        delete ui;

        delete _channels;
        delete _channelController;
    }
}
