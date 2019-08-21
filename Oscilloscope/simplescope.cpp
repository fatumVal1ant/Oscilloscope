#include "simplescope.h"
#include "ui_simplescope.h"

#include "dublicatechannel.h"
#include "ichannel.h"
#include "enums.h"

namespace oscilloscope {
    /// СОЗДАНИЕ ДИСПЛЕЯ

    SimpleScope::SimpleScope(QWidget *parent, const QString &name, GlobalChannelList *globalList) : QWidget(parent) { 
        ui = new Ui::SimpleScope;
        ui->setupUi(this);

        _globalList = globalList;
        _stop = false;

        // СОЗДАНИЕ ОБЪЕКТОВ

        _channels = new LocalChannelList(ui->channelsLayout);

        _display = new Display();
        ui->verticalLayout_3->insertWidget(0, _display);
        ui->verticalLayout_3->setStretch(0, 3);
        ui->verticalLayout_3->setStretch(1, 1);

        // УСТАНОВКА ЗНАЧЕНИЙ ПОУМОЛЧАНИЮ ОБЪЕКТАМ

        ui->DisplayName->setText(name);
        setWindowTitle(ui->DisplayName->text());

        ui->channelController->hide();

        ui->TriggerLevel->setEnabled(false);
        ui->TransformSpins->setEnabled(false);
        ui->TransformSpinDouble->hide();

        QMetaEnum metaEnum = QMetaEnum::fromType<Enums::Colors>();

        for (int i = 0; i < COLORS_COUNT; i++) {
            ui->ColorReal->addItem(metaEnum.key(i));
            ui->ColorImg->addItem(metaEnum.key(i));
        }

        metaEnum = QMetaEnum::fromType<Enums::TransformateType>();

        for (int i = 0; i < TRANSORM_COUNT; i++)
            ui->transormateType->addItem(metaEnum.key(i));

        metaEnum = QMetaEnum::fromType<Enums::TriggersType>();

        for (int i = 0; i < TRIGGERS_COUNT; i++)
            ui->TriggerType->addItem(metaEnum.key(i));

        // СОЗДАНИЕ МЕНЮ БАРА

        _bar = new QMenuBar(this);

        QMenu *menu = new QMenu();
        menu->setTitle("Вид");

        QAction *viewAct = new QAction("Сбросить маркеры", this);
        viewAct->setShortcut(Qt::Key_R);
        connect(viewAct, SIGNAL(triggered()), _display, SLOT(resetMarkers()));
        menu->addAction(viewAct);

        viewAct = new QAction("Сбросить вид", this);

        QList<QKeySequence> shortcuts;
        shortcuts.push_back(Qt::MiddleButton);
        shortcuts.push_back(Qt::Key_Tab);

        viewAct->setShortcuts(shortcuts);
        connect(viewAct, SIGNAL(triggered()), _display, SLOT(scaleByCenter()));
        menu->addAction(viewAct);

        _pauseAct = new QAction("Остановить обновления", this);
        _pauseAct->setShortcut(Qt::Key_Space);
        connect(_pauseAct, SIGNAL(triggered(bool)), this, SLOT(setPause()));
        menu->addAction(_pauseAct);

        _bar->addMenu(menu);
        _bar->show();

        // СОЕДИНЕНИЕ СИГНАЛОВ И СЛОТОВ

        connect(_channels, SIGNAL(itemChecked(QObject *)), this, SLOT(itemChecked(QObject *)));
        connect(_channels, SIGNAL(channelDeleted(QString)), this, SLOT(deleteChannel(QString)));

        connect(ui->ShowImg, SIGNAL(stateChanged(int)), this, SLOT(repaint()));
        connect(ui->ShowReal, SIGNAL(stateChanged(int)), this, SLOT(repaint()));

        connect(ui->TriggerLevel, SIGNAL(valueChanged(double)), this, SLOT(repaint()));

        connect(ui->TransformSpinDouble, SIGNAL(valueChanged(double)), this, SLOT(repaint()));
        connect(ui->TransformSpinInt, SIGNAL(valueChanged(int)), this, SLOT(repaint()));

        connect(ui->RealMaxPaint, SIGNAL(stateChanged(int)), this, SLOT(minMaxUpdate()));
        connect(ui->RealMinPaint, SIGNAL(stateChanged(int)), this, SLOT(minMaxUpdate()));
        connect(ui->ImagMaxPaint, SIGNAL(stateChanged(int)), this, SLOT(minMaxUpdate()));
        connect(ui->ImagMinPaint, SIGNAL(stateChanged(int)), this, SLOT(minMaxUpdate()));

        connect(_display, SIGNAL(markersRecounted()), this, SLOT(markersRecounted()));

        connect(ui->DisplayName, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));

        this->installEventFilter(this);
        setAttribute(Qt::WA_DeleteOnClose, true);
    }

    /// ОТРИСОВКА

    void SimpleScope::paintEvent(QPaintEvent *) {
        LocalChannelListView * channelsView = _channels->channelsView();
        QCheckBox *channel = dynamic_cast<QCheckBox *>(channelsView->itemWidget(channelsView->currentItem()));

        // ПОЛУЧЕНИЕ И ИЗМЕНЕНИЕ АТРИБУТОВ КАНАЛОВ

        if (channel) {
            iChannel *ch;

            int index = _globalList->indexByName(channel->text());
            if (index == -1) {
                index = _channels->indexByName(channel->text());
                ch = _channels->channels()->at(index);
            } else ch = _globalList->channels()->at(index);

            Attributes *atr = channelsView->attribute(channelsView->currentRow());
            QMetaEnum metaEnum = QMetaEnum::fromType<Enums::TransformateType>();

            if (ui->channelName->text() != channel->text()) {     
                // ОБНОВЛЕНИЕ ВЫВОДА ТЕКУЩИХ АТРИБУТОВ

                disconnect(ui->ShowImg, SIGNAL(stateChanged(int)), this, SLOT(repaint()));
                disconnect(ui->ShowReal, SIGNAL(stateChanged(int)), this, SLOT(repaint()));

                disconnect(ui->TriggerLevel, SIGNAL(valueChanged(double)), this, SLOT(repaint()));

                disconnect(ui->TransformSpinDouble, SIGNAL(valueChanged(double)), this, SLOT(repaint()));
                disconnect(ui->TransformSpinInt, SIGNAL(valueChanged(int)), this, SLOT(repaint()));

                ui->channelName->setText(channel->text());
                ui->channelController->show();

                ui->ColorReal->setCurrentIndex(colorIndex(atr->_colorReal));
                ui->ColorImg->setCurrentIndex(colorIndex(atr->_colorImg));

                ui->transormateType->setCurrentIndex(metaEnum.value(atr->_transformateType));

                metaEnum = QMetaEnum::fromType<Enums::TriggersType>();
                ui->TriggerType->setCurrentIndex(metaEnum.value(atr->_triggerType));

                ui->createDublicate->setEnabled(_globalList->indexByName(channel->text()) != -1);

                ui->TransformSpinInt->setValue(atr->_moveAvgCoef);
                ui->TransformSpinDouble->setValue(atr->_expSmthCoef);
                ui->TriggerLevel->setValue(atr->_triggerLevel);

                ui->ShowReal->setChecked(atr->_showReal);
                ui->ShowImg->setChecked(atr->_showImag);

                _display->setCurrentChannel(ch);

                connect(ui->ShowImg, SIGNAL(stateChanged(int)), this, SLOT(repaint()));
                connect(ui->ShowReal, SIGNAL(stateChanged(int)), this, SLOT(repaint()));

                connect(ui->TriggerLevel, SIGNAL(valueChanged(double)), this, SLOT(repaint()));

                connect(ui->TransformSpinDouble, SIGNAL(valueChanged(double)), this, SLOT(repaint()));
                connect(ui->TransformSpinInt, SIGNAL(valueChanged(int)), this, SLOT(repaint()));

                _display->markersRecount();
            } else {
                // ИЗМЕНЕНИЕ АТРИБУТОВ В СООТВЕТСТВИИ С ИХ ВЫВОДОМ

                QColor colorReal = colorByIndex(ui->ColorReal->currentIndex());
                QColor colorImg = colorByIndex(ui->ColorImg->currentIndex());

                QString metaName = ui->transormateType->currentText();
                QByteArray ba = metaName.toLocal8Bit();
                const char *metaKey = ba.data();

                int transformateType = metaEnum.keyToValue(metaKey);

                metaName = ui->TriggerType->currentText();
                ba = metaName.toLocal8Bit();
                metaKey = ba.data();

                metaEnum = QMetaEnum::fromType<Enums::TriggersType>();
                int triggerType = metaEnum.keyToValue(metaKey);

                if (colorReal != atr->_colorReal) {
                    atr->_colorReal = colorReal;

                    channelUpdate(findChannel(channel->text()), channel->text());
                }

                if (colorImg != atr->_colorImg) {
                    atr->_colorImg = colorImg;

                    channelUpdate(findChannel(channel->text()), channel->text());
                }

                if (transformateType != atr->_transformateType) {
                    atr->_transformateType =  static_cast<Enums::TransformateType>(transformateType);

                    recount(channel->text());
                }

                if (triggerType != atr->_triggerType) {
                    atr->_triggerType =  static_cast<Enums::TriggersType>(triggerType);

                    recount(channel->text());
                }

                if (ui->TriggerLevel->isEnabled() && atr->_triggerLevel != ui->TriggerLevel->value()) {
                    atr->_triggerLevel = ui->TriggerLevel->value();

                    recount(channel->text());
                }

                if (ui->TransformSpinInt->isEnabled() && !ui->TransformSpinInt->isHidden() && atr->_moveAvgCoef != ui->TransformSpinInt->value()) {
                    atr->_moveAvgCoef = ui->TransformSpinInt->value();

                    recount(channel->text());
                }

                if (ui->TransformSpinDouble->isEnabled() && !ui->TransformSpinDouble->isHidden() && atr->_expSmthCoef != ui->TransformSpinDouble->value()) {
                    atr->_expSmthCoef = ui->TransformSpinDouble->value();

                    recount(channel->text());
                }

                if (ui->ShowReal->isChecked() != atr->_showReal) {
                    atr->_showReal = ui->ShowReal->isChecked();

                    channelUpdate(findChannel(channel->text()), channel->text());
                }

                if (ui->ShowImg->isChecked() != atr->_showImag) {
                    atr->_showImag = ui->ShowImg->isChecked();

                    channelUpdate(findChannel(channel->text()), channel->text());
                }
            }

            // ПРОВЕРКА НА КОМПЛЕКСНОЕ ЧИСЛО

            if (ch->data()->frame()->_isComplex) {
                ui->ImgLayout->setEnabled(true);
                ui->ShowReal->setEnabled(true);;
            } else {
                ui->ImgLayout->setEnabled(false);

                ui->ShowReal->setEnabled(false);
                ui->ShowReal->setChecked(true);
            }

            // ПРОВЕРКА МЕТОДА ПРЕОБРАЗОВАНИЯ

            if (atr->_transformateType == Enums::TransformateType::ExponentialSmoothing) {
                ui->TransformSpins->setEnabled(true);

                ui->TransformSpinInt->hide();
                ui->TransformSpinDouble->show();    
            } else if (atr->_transformateType == Enums::TransformateType::MovingAverage) {
                ui->TransformSpins->setEnabled(true);

                ui->TransformSpinDouble->hide();
                ui->TransformSpinInt->show();
            } else ui->TransformSpins->setEnabled(false);

            ui->TriggerLevel->setEnabled(atr->_triggerType != Enums::TriggersType::WithoutTriggers);
        } else {
            _display->setCurrentChannel(nullptr);
            _display->deleteGraph("MinMaxPoint");

            ui->channelName->setText("Канал не выбран");
            ui->channelController->hide();
        }
    }

    /// СЛОТ ОБРАБОТКИ СИГНАЛА ЧЕКБОКСОВ В ЛОКАЛЬНОМ СПИСКЕ КАНАЛОВ

    void SimpleScope::itemChecked(QObject *object) {
        QCheckBox *channel = dynamic_cast<QCheckBox *>(object);

        if (channel->isChecked()) recount(channel->text());
            else {
                deleteChannel(channel->text());
                if ((dynamic_cast<QCheckBox *>(_channels->channelsView()->itemWidget(_channels->channelsView()->currentItem()))) == channel)
                    _display->deleteGraph("MinMaxPoint");
            }
    }

    /// ПОИСК КАНАЛА С ЕГО АТРИБУТАМИ И ЕГО СТАТУСОМ ПО ЕГО ИМЕНИ

    std::tuple<iChannel *, Attributes *, bool> SimpleScope::findChannel(const QString &_name) {
        LocalChannelListView * channelsView = _channels->channelsView();
        QCheckBox *channel;

        for (int i = 0; i < channelsView->count(); i++) {
            channel = dynamic_cast<QCheckBox *>(channelsView->itemWidget(channelsView->item(i)));

            if (channel->text() == _name) {
                if (channel->isChecked()) {
                    Attributes *atr = localList()->channelsView()->attribute(i);

                    iChannel *ch;
                    bool alive = true;

                    int index = _globalList->indexByName(channel->text());
                    if (index == -1) {
                        index = _channels->indexByName(channel->text());
                        ch = _channels->channels()->at(index);

                        QString name = ch->data()->frame()->_channelName;
                        index = _globalList->indexByName(name);
                        Channel *temp = dynamic_cast<Channel *>(_globalList->channels()->at(index));

                        alive = temp->status();
                    } else {
                        ch = _globalList->channels()->at(index);

                        Channel *temp = dynamic_cast<Channel *>(ch);
                        alive = temp->status();
                    }

                    return std::tuple<iChannel *, Attributes *, bool>(ch, atr, alive);
                }

                return std::tuple<iChannel *, Attributes *, bool>(nullptr, nullptr, false);
            }
        }

        return std::tuple<iChannel *, Attributes *, bool>(nullptr, nullptr, false);
    }

    /// ОБНОВЛЕНИЕ ВЫВОДА КАНАЛА

    void SimpleScope::channelUpdate(std::tuple<iChannel *, Attributes *, bool> _tuple, QString name) {
        iChannel *channel = std::get<0>(_tuple);

        if (channel) {
            Attributes *atr = std::get<1>(_tuple);
            bool alive = std::get<2>(_tuple);

            QVector<std::complex<double>> points = channel->points();
            QVector<double> offsetX = channel->offsetX();

            double divY = channel->data()->frame()->_divYValue;

            if (points.count() > 0) {
                // ОТРИСОВКА ДЕЙСТВИТЕЛЬНОЙ ЧАСТИ

                if (atr->_showReal) {
                    QLineSeries *series = new QLineSeries();
                    series->setName(name);

                    for (int j = 0; j < points.size(); j++)
                        series->append(offsetX.at(j), points.at(j).real() * divY);

                    series->setUseOpenGL(false);

                    QColor color = atr->_colorReal;
                    if (!alive) color.setAlphaF(0.2);
                        else color.setAlphaF(1);

                    series->setPen(QPen(color, 1));

                    _display->addGraph(series);
                } else _display->deleteGraph(name);

                // ОТРИСОВКА МНИМОЙ ЧАСТИ

                if (atr->_showImag && channel->data()->frame()->_isComplex) {
                    QLineSeries *series = new QLineSeries();
                    series->setName(name + "IMAG");

                    for (int j = 0; j < points.size(); j++)
                        series->append(offsetX.at(j), points.at(j).imag() * divY);

                    series->setUseOpenGL(false);

                    QColor color = atr->_colorImg;
                    if (!alive) color.setAlphaF(0.2);
                        else color.setAlphaF(1);

                    series->setPen(QPen(color, 1));

                    _display->addGraph(series);
                } else _display->deleteGraph(name + "IMAG");
            }
        };
    }

    /// ПЕРЕСЧЕТ ТОЧЕК ГРАФИКА КАНАЛА

    void SimpleScope::recount(const QString &name) {
        std::tuple<iChannel *, Attributes *, bool> _tuple = findChannel(name);

        iChannel *channel = std::get<0>(_tuple);

        if (channel) {
            Attributes *atr = std::get<1>(_tuple);

            channel->trigger(atr->_triggerType, atr->_triggerLevel);
            channel->transform(atr->_transformateType, atr->_expSmthCoef, atr->_moveAvgCoef);

            channelUpdate(_tuple, name);

            _display->markersRecount();
        }
    }

    /// ПЕРЕСЧЕТ ТОЧЕК ГРАФИКА ДУБЛИКАТОВ КАНАЛА ПО ИМЕНИ ОРИГАНАЛА

    void SimpleScope::recountDublicates(const QString &_name) {
        LocalChannelListView * channelsView = _channels->channelsView();
        QCheckBox *channel;

        for (int i = 0; i < channelsView->count(); i++) {
            channel = dynamic_cast<QCheckBox *>(channelsView->itemWidget(channelsView->item(i)));

            if (channel->text().contains(DUBLICATE_NAME_BY_PARENT(_name), Qt::CaseInsensitive) && channel->isChecked()) {
                Attributes *atr = localList()->channelsView()->attribute(i);

                iChannel *ch;
                bool alive = true;

                int index = _channels->indexByName(channel->text());
                ch = _channels->channels()->at(index);

                QString name = ch->data()->frame()->_channelName;
                index = _globalList->indexByName(name);

                Channel *temp = dynamic_cast<Channel *>(_globalList->channels()->at(index));
                alive = temp->status();

                ch->trigger(atr->_triggerType, atr->_triggerLevel);
                ch->transform(atr->_transformateType, atr->_expSmthCoef, atr->_moveAvgCoef);

                channelUpdate(std::tuple<iChannel *, Attributes *, bool>(ch, atr, alive), channel->text());

                _display->markersRecount();
            }
        }
    }

    /// ПОЛУЧЕНИЕ ЛОКАЛЬНОГО СПИСКА КАНАЛОВ ДИСПЛЕЯ

    LocalChannelList *SimpleScope::localList() const {
        return _channels;
    }

    /// ИНДЕКС ЦВЕТА В СООТВЕТСТВИИ С ТАБЛИЦЕЙ

    int SimpleScope::colorIndex(const QColor &color) const {
        if (color == Qt::black) return Enums::Colors::Black;
        else if (color == Qt::red) return Enums::Colors::Red;
        else if (color == Qt::blue) return Enums::Colors::Blue;
        else if (color == Qt::green) return Enums::Colors::Green;
        else return Enums::Colors::Yellow;
    }

    /// ОПРЕДЕЛЕНИЕ ЦВЕТА ПО ИНДЕКСУ В СООТВЕТСТВИИ С ТАБЛИЦЕЙ

    const QColor SimpleScope::colorByIndex(int index) const {
        if (index == Enums::Colors::Black) return Qt::black;
        else if (index == Enums::Colors::Red) return Qt::red;
        else if (index == Enums::Colors::Blue) return Qt::blue;
        else if (index == Enums::Colors::Green) return Qt::green;
        else return Qt::yellow;
    }

    /// УДАЛЕНИЕ ГРАФИКА С ДИСПЛЕЯ

    void SimpleScope::deleteChannel(const QString &name) {
        _display->deleteGraph(name);
        _display->deleteGraph(name + "IMAG");
    }

    void SimpleScope::deleteDublicates(const QString &name) {
        _display->deleteDublicatesGraph(name);
    }

    /// НАЖАТИЕ КНОПКИ - СОЗДАНИЕ ДУБЛИКАТА КАНАЛА

    void SimpleScope::on_createDublicate_pressed() {
        LocalChannelListView * channelsView = _channels->channelsView();
        QCheckBox *item = dynamic_cast<QCheckBox *>(channelsView->itemWidget(channelsView->currentItem()));

        if (item) {
            int index = _globalList->indexByName(item->text());

            Channel *channel = static_cast<Channel *>(_globalList->channels()->at(index));
            DublicateChannel *dublicate = new DublicateChannel(channel);

            _channels->add(dublicate);
        }

    }

    /// ГЕТТЕР СТАТУСА ПАУЗЫ

    bool SimpleScope::pause() const {
        return _stop;
    }

    /// ПАУЗА ОБНОВЛЕНИЙ ДАННЫХ

    void SimpleScope::setPause() {
        _stop = !_stop;
        if (_stop) _pauseAct->setText("Возобновить обновления");
            else _pauseAct->setText("Остановить обновления");
        updateTitle();
    }

    /// ФИЛЬТР СОБЫТИЙ

    bool SimpleScope::eventFilter(QObject *object, QEvent *event) {
        if (event->type() == QEvent::WindowDeactivate) {
            if (object == this)
                _display->keysReset();
        } else if (event->type() == QEvent::Resize) {
            if (object == this)
                _display->resetMarkers();
        }

        return false;
    }

    /// ПЕРЕСЧЕТ МАРКЕРОВ

    void SimpleScope::markersRecounted() {
        QVector<QPointF *> points = _display->minAndMax();

        ui->RealExpectedValue->setText(QString::number(_display->expectedValue().real(), 'f', 2));
        ui->RealStandardDeviation->setText(QString::number(_display->standardDeviation().real(), 'f', 2));
        ui->RealCapacity->setText(QString::number(_display->capacity().real(), 'f', 2));
        ui->RealAmplitude->setText(QString::number(_display->amplitude().real(), 'f', 2));

        ui->RealMinValue->setText(QString::number(points.at(0)->x(), 'f', 2) + " : " + QString::number(points.at(0)->y(), 'f', 2));
        ui->RealMaxValue->setText(QString::number(points.at(2)->x(), 'f', 2) + " : " + QString::number(points.at(2)->y(), 'f', 2));

        ui->ImagExpectedValue->setText(QString::number(_display->expectedValue().imag(), 'f', 2));
        ui->ImagStandardDeviation->setText(QString::number(_display->standardDeviation().imag(), 'f', 2));
        ui->ImagCapacity->setText(QString::number(_display->capacity().imag(), 'f', 2));
        ui->ImagAmplitude->setText(QString::number(_display->amplitude().imag(), 'f', 2));

        ui->ImagMinValue->setText(QString::number(points.at(1)->x(), 'f', 2) + " : " + QString::number(points.at(1)->y(), 'f', 2));
        ui->ImagMaxValue->setText(QString::number(points.at(3)->x(), 'f', 2) + " : " + QString::number(points.at(3)->y(), 'f', 2));

        ui->Amplitude->setText(QString::number(_display->complexAmplitude(), 'f', 2));

        minMaxUpdate();
    }

    /// ОБНОВЛЕНИЕ ТОЧЕК ЭКСТРЕМУМА

    void SimpleScope::minMaxUpdate() {
        _display->deleteGraph("MinMaxPoint");

        if (_display->minAndMax().size() > 0) {
            LocalChannelListView * channelsView = _channels->channelsView();
            QCheckBox *channel = dynamic_cast<QCheckBox *>(channelsView->itemWidget(channelsView->currentItem()));

            if (channel->isChecked()) {
                QVector<QPointF *> points = _display->minAndMax();

                if (ui->RealMinPaint->isChecked() || ui->RealMaxPaint->isChecked() || ui->ImagMinPaint->isChecked() || ui->ImagMaxPaint->isChecked()) {
                    QScatterSeries *series = new QScatterSeries();

                    series->setName("MinMaxPoint");
                    series->setUseOpenGL(false);
                    series->setMarkerSize(10);

                    if (ui->RealMinPaint->isChecked())
                        series->append(points.at(0)->x(), points.at(0)->y());

                    if (ui->RealMaxPaint->isChecked())
                        series->append(points.at(2)->x(), points.at(2)->y());

                    if (ui->ImagMinPaint->isChecked())
                        series->append(points.at(1)->x(), points.at(1)->y());

                    if (ui->ImagMaxPaint->isChecked())
                        series->append(points.at(3)->x(), points.at(3)->y());

                    _display->addGraph(series);
                }
            }
        }
    }

    /// ОБНОВЛЕНИЕ ЗАГОЛОВКА

    void SimpleScope::updateTitle() {
        if (_stop) setWindowTitle(ui->DisplayName->text() + " [ПАУЗА]");
            else setWindowTitle(ui->DisplayName->text());
    }

    /// ДЕСТРУКТОР

    SimpleScope::~SimpleScope() {
        delete _channels;
        delete ui;
    }
}
