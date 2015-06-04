#include "BluetoothClient.h"
#include "ui_BluetoothClient.h"
#include <QFile>
#include <QTime>

BluetoothClient::BluetoothClient(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::BluetoothClient),
    _currentMode(Crutches),
    _currentSpeedTimeout(FirstSpeed),
    _timerIdHookWarning(0),
    _timerIdTemperatureWarning(0),
    _crutchOn(true),
    _pillarOn(true),
    _hookOn(true),
    _derricOn(true),
    _telBoomOn(true),
    _outriggerOn(true),
    _hookWarningOn(true),
    _temperatureWarningOn(true),
    _localSliderChanged(0),
    _timerIdSliderControls(0),
    _simulation(false),
    _controls(Sliders)
{
    _ui->setupUi(this);
//    _ui->_labelConnecting->setText("<FONT COLOR = RED>Not connected.</FONT>");
//    _ui->_labelConnecting->setStyleSheet("color: red");
    _ui->_pushButtonConnecting->setStyleSheet("color: red");
    _ui->_pushButtonConnecting->setText("Not connected");

    _ui->_pushButtonPower->setIconSize(_ui->_pushButtonPower->size()-QSize(10,10));
    _ui->_pushButtonLight->setIconSize(_ui->_pushButtonLight->size()-QSize(10,10));
    _ui->_pushButtonSoundSignal->setIconSize(_ui->_pushButtonSoundSignal->size()-QSize(10,10));
    _ui->_pushButtonHookWarning->setIconSize(_ui->_pushButtonHookWarning->size()-QSize(10,10));
    _ui->_pushButtonTemperatureHigh->setIconSize(_ui->_pushButtonTemperatureHigh->size()-QSize(10,10));
    _ui->_pushButtonSpeed->setIconSize(_ui->_pushButtonSpeed->size()-QSize(10,10));
    _ui->_pushButtonCrutchesOrPillar->setIconSize(_ui->_pushButtonCrutchesOrPillar->size()-QSize(10,10));
    _ui->_pushButtonPillarLabel->setIconSize(_ui->_pushButtonPillarLabel->size()-QSize(10,10));
    _ui->_pushButtonHookLabel->setIconSize(_ui->_pushButtonHookLabel->size()-QSize(10,10));
    _ui->_pushButtonDerrickLabel->setIconSize(_ui->_pushButtonDerrickLabel->size()-QSize(10,10));
    _ui->_pushButtonOutriggerLabel->setIconSize(_ui->_pushButtonOutriggerLabel->size()-QSize(10,10));
    _ui->_pushButtonTelBoomLabel->setIconSize(_ui->_pushButtonTelBoomLabel->size()-QSize(10,10));

    int minusValue=80;
#ifdef ANDROID_233
    minusValue=10;
#endif
    _ui->_pushButtonPillarUp->setIconSize(_ui->_pushButtonPillarUp->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonPillarDown->setIconSize(_ui->_pushButtonPillarDown->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonDerrickUp->setIconSize(_ui->_pushButtonDerrickUp->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonDerrickDown->setIconSize(_ui->_pushButtonDerrickDown->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonOutriggerUp->setIconSize(_ui->_pushButtonOutriggerUp->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonOutriggerDown->setIconSize(_ui->_pushButtonOutriggerDown->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonTelBoomUp->setIconSize(_ui->_pushButtonTelBoomUp->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonTelBoomDown->setIconSize(_ui->_pushButtonTelBoomDown->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonHookUp->setIconSize(_ui->_pushButtonHookUp->size()-QSize(minusValue,minusValue));
    _ui->_pushButtonHookDown->setIconSize(_ui->_pushButtonHookDown->size()-QSize(minusValue,minusValue));

    QFile file(":/simple.qss");
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    qApp->setStyleSheet(strCSS);

    _ui->_labelConnecting->hide();

    //скрываем слайдеры или кнопки в зависимости от выбранного режима
    if(_controls==Buttons)
    {
        _ui->_verticalSliderPillar->hide();
        _ui->_verticalSliderDerrick->hide();
        _ui->_verticalSliderOutrigger->hide();
        _ui->_verticalSliderTelBoom->hide();
        _ui->_verticalSliderHook->hide();
    }
    else
    {
        _ui->_pushButtonPillarUp->hide();
        _ui->_pushButtonPillarDown->hide();
        _ui->_pushButtonDerrickUp->hide();
        _ui->_pushButtonDerrickDown->hide();
        _ui->_pushButtonOutriggerUp->hide();
        _ui->_pushButtonOutriggerDown->hide();
        _ui->_pushButtonTelBoomUp->hide();
        _ui->_pushButtonTelBoomDown->hide();
        _ui->_pushButtonHookUp->hide();
        _ui->_pushButtonHookDown->hide();
    }

    hidePillarControls();       //переключаем в режим "Стойки" (прячем управление Pillar)
    setEnabledControls(false);  //отключаем "питание" у пульта

    // соединяем обработчики отпускания слайдера со слайдером
    connect( _ui->_verticalSliderPillar, SIGNAL(sliderReleased()),
            this, SLOT(slotSliderAfterReleased()) );
    connect( _ui->_verticalSliderDerrick, SIGNAL(sliderReleased()),
            this, SLOT(slotSliderAfterReleased()) );
    connect( _ui->_verticalSliderOutrigger, SIGNAL(sliderReleased()),
            this, SLOT(slotSliderAfterReleased()) );
    connect( _ui->_verticalSliderTelBoom, SIGNAL(sliderReleased()),
            this, SLOT(slotSliderAfterReleased()) );
    connect( _ui->_verticalSliderHook, SIGNAL(sliderReleased()),
            this, SLOT(slotSliderAfterReleased()) );

    // соединяем обработчики нажатия слайдера со слайдером
    connect( _ui->_verticalSliderPillar, SIGNAL(sliderPressed()),
            this, SLOT(slotSliderAfterPressed()) );
    connect( _ui->_verticalSliderDerrick, SIGNAL(sliderPressed()),
            this, SLOT(slotSliderAfterPressed()) );
    connect( _ui->_verticalSliderOutrigger, SIGNAL(sliderPressed()),
            this, SLOT(slotSliderAfterPressed()) );
    connect( _ui->_verticalSliderTelBoom, SIGNAL(sliderPressed()),
            this, SLOT(slotSliderAfterPressed()) );
    connect( _ui->_verticalSliderHook, SIGNAL(sliderPressed()),
            this, SLOT(slotSliderAfterPressed()) );

    // соединяем обработчики изменения значения слайдера со слайдером
    connect( _ui->_verticalSliderPillar, SIGNAL(valueChanged(int)),
            this, SLOT(slotSliderAfterValueChanged(int)));
    connect( _ui->_verticalSliderDerrick, SIGNAL(valueChanged(int)),
            this, SLOT(slotSliderAfterValueChanged(int)));
    connect( _ui->_verticalSliderOutrigger, SIGNAL(valueChanged(int)),
            this, SLOT(slotSliderAfterValueChanged(int)));
    connect( _ui->_verticalSliderTelBoom, SIGNAL(valueChanged(int)),
            this, SLOT(slotSliderAfterValueChanged(int)));
    connect( _ui->_verticalSliderHook, SIGNAL(valueChanged(int)),
            this, SLOT(slotSliderAfterValueChanged(int)));

    _timerIdCrutchesAndPillar = startTimer(300);
    _ui->_pushButtonPillarLabel->setIcon(QIcon(":/pics/left_crutch_on.svg"));
    _ui->_pushButtonHookLabel->setIcon(QIcon(":/pics/right_crutch_on.svg"));

    _timerIdDerricAndTelBoom = startTimer(350);
    _ui->_pushButtonDerrickLabel->setIcon(QIcon(":/pics/derrick_white.svg"));
    _ui->_pushButtonTelBoomLabel->setIcon(QIcon(":/pics/telescopic_boom_black.svg"));

    _timerIdOutrigger = startTimer(400);
    _ui->_pushButtonOutriggerLabel->setIcon(QIcon(":/pics/outrigger_white.svg"));

    _buuid = QBluetoothUuid(QBluetoothUuid::SerialPort);
//    _buuid = QBluetoothUuid(serviceUuid16);  // если будем подключаться через собственную службу c uuid = serviceUuid16
    _socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    connect(_socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(_socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(readError(QBluetoothSocket::SocketError)));
}

BluetoothClient::~BluetoothClient()
{
    delete _socket;
    delete _ui;
}

void BluetoothClient::timerEvent(QTimerEvent *event)
{
    int id = event->timerId();
    if(id == _timerIdCrutchesAndPillar)
    {
        if(_currentMode==Crutches)
        {
            setCrutchesLabels();
            _crutchOn=!_crutchOn;
        }
        else //if(_currentMode==Pillar)
        {
            setPillarAndHookLabels();
            _pillarOn=!_pillarOn;
            _hookOn=!_hookOn;
        }
    }
    else if(id == _timerIdDerricAndTelBoom)
    {
        if(_derricOn)
            _ui->_pushButtonDerrickLabel->setIcon(QIcon(":/pics/derrick_black.svg"));
        else
            _ui->_pushButtonDerrickLabel->setIcon(QIcon(":/pics/derrick_white.svg"));
        _derricOn=!_derricOn;
        if(_telBoomOn)
            _ui->_pushButtonTelBoomLabel->setIcon(QIcon(":/pics/telescopic_boom_black.svg"));
        else
            _ui->_pushButtonTelBoomLabel->setIcon(QIcon(":/pics/telescopic_boom_white.svg"));
        _telBoomOn=!_telBoomOn;
    }
    else if(id == _timerIdOutrigger)
    {
        if(_outriggerOn)
            _ui->_pushButtonOutriggerLabel->setIcon(QIcon(":/pics/outrigger_black.svg"));
        else
            _ui->_pushButtonOutriggerLabel->setIcon(QIcon(":/pics/outrigger_white.svg"));
        _outriggerOn=!_outriggerOn;
    }
    else if(id == _timerIdErrorMessage)
    {
        killTimer(_timerIdErrorMessage);
        _ui->_labelError->clear();
    }
    else if(id == _timerIdHookWarning)
    {
        if(_hookWarningOn)
            _ui->_pushButtonHookWarning->setIcon(QIcon(":/pics/warning_red.svg"));
        else
            _ui->_pushButtonHookWarning->setIcon(QIcon(":/pics/warning_normal.svg"));
        _hookWarningOn=!_hookWarningOn;
    }
    else if(id == _timerIdTemperatureWarning)
    {
        if(_temperatureWarningOn)
            _ui->_pushButtonTemperatureHigh->setIcon(QIcon(":/pics/temperature_high_red.svg"));
        else
            _ui->_pushButtonTemperatureHigh->setIcon(QIcon(":/pics/temperature_normal.svg"));
        _temperatureWarningOn=!_temperatureWarningOn;
    }
    else if(_mapTimerIdMessages.contains(id))
    {
        Element el = _mapTimerIdMessages[id].element;
        quint8 mes = _mapTimerIdMessages[id].message;
        sendMessage(el, mes);
    }
    else if(id == _timerIdSliderControls)
    {
        Element el = _currentMessage.element;
        quint8 mes = _currentMessage.message;
        sendMessage(el, mes);
    }

}

void BluetoothClient::readError(QBluetoothSocket::SocketError err)
{
    switch (err) {
    case QBluetoothSocket::UnknownSocketError:
        _ui->_labelError->setText("An unknown error has occurred.");
        _timerIdErrorMessage = startTimer(ErrorMessageDuration);
        break;
    case QBluetoothSocket::NoSocketError:
        _ui->_labelError->setText("No error. Used for testing.");
        _timerIdErrorMessage = startTimer(ErrorMessageDuration);
        break;
    case QBluetoothSocket::HostNotFoundError:
        _ui->_labelError->setText("Could not find the remote host.");
        _timerIdErrorMessage = startTimer(ErrorMessageDuration);
        break;
    case QBluetoothSocket::ServiceNotFoundError:
        _ui->_labelError->setText("Could not find the service UUID on remote host.");
        _timerIdErrorMessage = startTimer(ErrorMessageDuration);
        break;
    case QBluetoothSocket::NetworkError:
        _ui->_labelError->setText("Attempt to read or write from socket returned an error");
        _timerIdErrorMessage = startTimer(ErrorMessageDuration);
        break;
    case QBluetoothSocket::UnsupportedProtocolError:
        _ui->_labelError->setText("The Protocol is not supported on this platform.");
        _timerIdErrorMessage = startTimer(ErrorMessageDuration);
        break;
    case QBluetoothSocket::OperationError:
        _ui->_labelError->setText("An operation was attempted while the socket was in a state that did not permit it.");
        _timerIdErrorMessage = startTimer(ErrorMessageDuration);
        break;
    default:
        break;
    }
}

void BluetoothClient::connected()
{
    QString label = "Connected to server: " + _socket->peerName();
//    _ui->_labelConnecting->setText("<FONT COLOR = GREEN>"+label+"</FONT>");
    _ui->_pushButtonConnecting->setStyleSheet("color: green");
    _ui->_pushButtonConnecting->setText(label);

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
//    qDebug() << "begin: " << quint64(Preamble);
//    qDebug() << "name: " << _localName;
//    qDebug() << "address: " << _localAddress;
//    qDebug() << "end: " << quint64(Preamble);

    out << quint64(Preamble) << _localName << _localAddress << quint64(Preamble);

    writeInSocket(arrBlock);
}

void BluetoothClient::disconnected()
{
    QString label = "Server " + _socket->peerName()+ " disconnected";
//    _ui->_labelConnecting->setText("<FONT COLOR = RED>"+label+"</FONT>");
    _ui->_pushButtonConnecting->setStyleSheet("color: red");
    _ui->_pushButtonConnecting->setText(label);
}

void BluetoothClient::readSocket()
{
    if (!_socket)
        return;
    QByteArray arr = _socket->readAll();
    if(QByteArray(_arr+arr).size()>=2)
    {
        while(arr.size()>0)
        {
            QByteArray newArr(arr);
            if(_arr.size()==0)
            {
                newArr.truncate(2);
                _arr=newArr;
                arr.remove(0,2);
            }
            else
            {   //if(_arr.size()==1)
                newArr.truncate(1);
                _arr=_arr+newArr;
                arr.remove(0,1);
            }
            if(_arr.size()>=2)
            {
                QDataStream in(&_arr, QIODevice::ReadOnly);
                quint8 el;
                quint8 mes;
                in >> el >> mes;
//                emit receivedMessage((Element)el, mes);
//                qDebug() << _arr.toHex();
                readMessage((Element)el, mes);
                _arr.clear();
            }
        }
    }
    else
       _arr+=arr;


//    QByteArray arr;
//    arr = _socket->readAll();
//    QDataStream in(&arr, QIODevice::ReadOnly);
//    quint8 el;
//    quint8 mes;
//    in >> el >> mes;
//    qDebug() << arr.toHex();
//    readMessage((Element)el, mes );
}

void BluetoothClient::writeInSocket(QByteArray &arr)
{
    qDebug() << "write: " << (_simulation? "[loc]": "")
             << arr.toHex() << "[" << arr.size() << "]";
    if(!_simulation)
        _socket->write(arr);

//    qDebug() << "write: " <<
//#ifdef LOCAL_SIMULATE
//                "[loc]"
//#else
//                ""
//#endif
//             << arr.toHex() << "[" << arr.size() << "]";
//#ifndef LOCAL_SIMULATE
//    _socket->write(arr);
//#endif
}

void BluetoothClient::readMessage(Element el, quint8 mes)
{
    switch (el) {
    case powerStatus:
        if(mes==0x01)
            setPowerOn(true);
        else
            setPowerOn(false);
        break;
    case lightStatus:
        if(mes==0x01)
            setLightOn(true);
        else
            setLightOn(false);
        break;
    case highTemperature:
        if(mes==0x01)
            setTemperatureWarning(true);
        else
            setTemperatureWarning(false);
        break;
    case hookWarning:
        if(mes==0x01)
            setHookWarning(true);
        else
            setHookWarning(false);
        break;
    default:
        break;
    }
}

void BluetoothClient::sendMessage(Element el, quint8 mes)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint8(el) << mes;

    writeInSocket(arrBlock);
}


void BluetoothClient::moveElement(Element el, quint8 mes)
{
    if(mes!=0x00) //кнопка была нажата
    {
        if(_controls==Buttons)
        {
            if(mes==0x01)
            {
                sendMessage(el, mes);
                int id = startTimer(_currentSpeedTimeout);
                _mapTimerIdMessages.insert(id, Message(el, mes));
            }
        }
        else
        {
            if(_timerIdSliderControls!=0)
                killTimer(_timerIdSliderControls);
            sendMessage(el, mes);
            _timerIdSliderControls = startTimer(_currentSpeedTimeout);
        }
        //... if(mes==0x02), if(mes==0x03), etc.
    }
    else //if (mes==0x00) //кнопка была отпущена
    {
        if(_controls==Buttons)
        {
            QMap<int, Message>::const_iterator it = _mapTimerIdMessages.constBegin();
            while(it!=_mapTimerIdMessages.constEnd())
            {
                if(it.value().element==el)
                {
                    killTimer(it.key());
                    _mapTimerIdMessages.remove(it.key());
                    break;
                }
                ++it;
            }
        }
        else
        {
            if(_timerIdSliderControls!=0)
            {
                killTimer(_timerIdSliderControls);
                _timerIdSliderControls=0;
            }
        }
    }
}

void BluetoothClient::setAddress(QBluetoothAddress addr)
{
    if(!_simulation)
        _socket->connectToService(addr, _buuid);
}

void BluetoothClient::setAddress(QString addr)
{
    if(!_simulation)
        _socket->connectToService(QBluetoothAddress(addr), _buuid);
}

void BluetoothClient::setLocalNameAndAddress(QString name, QString addr)
{
    _localName = name;
    _localAddress = addr;
}

void BluetoothClient::setSimulationMode(bool b)
{
    _simulation=b;
//    if(_ui->_pushButtonCrutchesOrPillar->isChecked())
//        _ui->_pushButtonCrutchesOrPillar->click();
    if(_simulation)
    {
        if(_socket->state()!=QBluetoothSocket::UnconnectedState)
            _socket->disconnectFromService();
        QString label = "Simulation mode";
        _ui->_pushButtonConnecting->setStyleSheet("color: royalblue");
        _ui->_pushButtonConnecting->setText(label);
    }
    if(_ui->_pushButtonPower->isChecked())
        setEnabledControls(true);
    else
        setEnabledControls(false);

}

void BluetoothClient::setControls(Controls c)
{
    _controls=c;
    //скрываем слайдеры или кнопки в зависимости от выбранного режима
    if(_currentMode==Crutches)
    {
//        qDebug() << "Crutches";
        hidePillarControls();
    }
    else
    {
//        qDebug() << "Pillar";
        showPillarControls();
    }
}

void BluetoothClient::on__pushButtonPower_clicked(bool checked)
{
//    qDebug() << checked;
//    checked == true - питание вкл.
    if(checked)
        sendMessage(powerButton, 0x01);
    else
        sendMessage(powerButton, 0x00);

    bool b = false;
    if(_simulation)
        b=true;

//#ifdef LOCAL_SIMULATE
//    b=true;
//#endif
#ifdef TURN_IMMEDIATELY
    b=true;
#endif

    if(b)
        setPowerOn(checked); // установить переключатель не дожидаясь ответа от КМУ
}

void BluetoothClient::setPowerOn(bool b)
{
    if(b)
    {
        _ui->_pushButtonPower->setStyleSheet("background-color: rgba(0, 255, 0, 10%);");
        _ui->_pushButtonPower->setIcon(QIcon(":/pics/power_green.svg"));

        setEnabledControls(true);
    }
    else
    {
        _ui->_pushButtonPower->setStyleSheet("background-color: rgba(255, 0, 0, 15%);");
        _ui->_pushButtonPower->setIcon(QIcon(":/pics/power_red.svg"));

        setEnabledControls(false);
    }
}

void BluetoothClient::on__pushButtonSoundSignal_pressed()
{
    sendMessage(soundSignal, 0x01);
    _ui->_pushButtonSoundSignal->setIcon(QIcon(":/pics/sound_signal_on.svg"));
}

void BluetoothClient::on__pushButtonSoundSignal_released()
{
    sendMessage(soundSignal, 0x00);
    _ui->_pushButtonSoundSignal->setIcon(QIcon(":/pics/sound_signal_off.svg"));
}

void BluetoothClient::on__pushButtonLight_clicked(bool checked)
{
    //    qDebug() << checked;
    //    checked == true - свет вкл.
    if(checked)
        sendMessage(lightButton, 0x01);
    else
        sendMessage(lightButton, 0x00);

    bool b = false;
    if(_simulation)
        b=true;
//#ifdef LOCAL_SIMULATE
//    b=true;
//#endif
#ifdef TURN_IMMEDIATELY
    b=true;
#endif
    if(b)
        setLightOn(checked); // установить переключатель не дожидаясь ответа от КМУ
}

void BluetoothClient::setLightOn(bool b)
{
    if(b)
    {
        _ui->_pushButtonLight->setStyleSheet("background-color: rgba(255, 204, 51, 60%)");
        _ui->_pushButtonLight->setIcon(QIcon(":/pics/light_on.svg"));
    }
    else
    {
        _ui->_pushButtonLight->setStyleSheet("background-color: rgba(255, 204, 51, 20%)");
        _ui->_pushButtonLight->setIcon(QIcon(":/pics/light_off.svg"));
    }
}


void BluetoothClient::setHookWarning(bool b)
{
    if(b)
    {
        if(_timerIdHookWarning!=0)
            killTimer(_timerIdHookWarning);
        _timerIdHookWarning = startTimer(WarningBlinkInterval);
        _ui->_pushButtonHookWarning->setIcon(QIcon(":/pics/warning_red.svg"));
    }
    else
    {
        killTimer(_timerIdHookWarning);
        _timerIdHookWarning=0;
        _ui->_pushButtonHookWarning->setIcon(QIcon(":/pics/warning_normal.svg"));
    }
}

void BluetoothClient::setTemperatureWarning(bool b)
{
    if(b)
    {
        if(_timerIdTemperatureWarning!=0)
            killTimer(_timerIdTemperatureWarning);
        _timerIdTemperatureWarning = startTimer(WarningBlinkInterval);
        _ui->_pushButtonTemperatureHigh->setIcon(QIcon(":/pics/temperature_high_red.svg"));
    }
    else
    {
        killTimer(_timerIdTemperatureWarning);
        _timerIdTemperatureWarning=0;
        _ui->_pushButtonTemperatureHigh->setIcon(QIcon(":/pics/temperature_normal.svg"));
    }
}

void BluetoothClient::on__pushButtonSpeed_clicked(bool checked)
{
    //    qDebug() << checked;
    //    checked == true - быстро
    if(checked)
    {
//        sendMessage(speedButton, 0x01);
        _currentSpeedTimeout = SecondSpeed;
        _ui->_pushButtonSpeed->setIcon(QIcon(":/pics/speed_high.svg"));
    }
    else
    {
        _currentSpeedTimeout = FirstSpeed;
//        sendMessage(speedButton, 0x00);
        _ui->_pushButtonSpeed->setIcon(QIcon(":/pics/speed_low.svg"));
    }
}

void BluetoothClient::on__pushButtonCrutchesOrPillar_clicked(bool checked)
{
    //    qDebug() << checked;
    //    checked == true - стойка, стрела, лебедка
    if(checked)
    {
        _currentMode = Pillar;
        setPillarAndHookLabels();
        _ui->_labelCrutchesOrPillar->setText("Cтойка, стрела, лебедка");
        showPillarControls();
    }
    else
    {
        _currentMode = Crutches;
        setCrutchesLabels();
        _ui->_labelCrutchesOrPillar->setText("Опоры");
        hidePillarControls();
    }
}


void BluetoothClient::setCrutchesLabels()
{
    if(_crutchOn)
    {
        _ui->_pushButtonPillarLabel->setIcon(QIcon(":/pics/left_crutch_off.svg"));
        _ui->_pushButtonHookLabel->setIcon(QIcon(":/pics/right_crutch_off.svg"));
    }
    else
    {
        _ui->_pushButtonPillarLabel->setIcon(QIcon(":/pics/left_crutch_on.svg"));
        _ui->_pushButtonHookLabel->setIcon(QIcon(":/pics/right_crutch_on.svg"));
    }
}


void BluetoothClient::setPillarAndHookLabels()
{
    if(_pillarOn)
        _ui->_pushButtonPillarLabel->setIcon(QIcon(":/pics/pillar_black.svg"));
    else
        _ui->_pushButtonPillarLabel->setIcon(QIcon(":/pics/pillar_white.svg"));

    if(_hookOn)
        _ui->_pushButtonHookLabel->setIcon(QIcon(":/pics/hook_black.svg"));
    else
        _ui->_pushButtonHookLabel->setIcon(QIcon(":/pics/hook_white.svg"));

}

void BluetoothClient::showPillarControls()
{
    _ui->_pushButtonCrutchesOrPillar->setIcon(QIcon(":/pics/pillar_boom_hook.svg"));
    _ui->_pushButtonDerrickLabel->setVisible(true);
    _ui->_pushButtonOutriggerLabel->setVisible(true);
    _ui->_pushButtonTelBoomLabel->setVisible(true);
    if(_controls==Buttons)
    {
        _ui->_verticalSliderPillar->hide();
        _ui->_verticalSliderDerrick->hide();
        _ui->_verticalSliderOutrigger->hide();
        _ui->_verticalSliderTelBoom->hide();
        _ui->_verticalSliderHook->hide();

        _ui->_pushButtonPillarUp->setIcon(QIcon(":/pics/rotate_up.svg"));
        _ui->_pushButtonPillarDown->setIcon(QIcon(":/pics/rotate_down.svg"));

        _ui->_pushButtonPillarUp->setVisible(true);
        _ui->_pushButtonPillarDown->setVisible(true);
        _ui->_pushButtonDerrickUp->setVisible(true);
        _ui->_pushButtonDerrickDown->setVisible(true);
        _ui->_pushButtonOutriggerUp->setVisible(true);
        _ui->_pushButtonOutriggerDown->setVisible(true);
        _ui->_pushButtonTelBoomUp->setVisible(true);
        _ui->_pushButtonTelBoomDown->setVisible(true);
        _ui->_pushButtonHookUp->setVisible(true);
        _ui->_pushButtonHookDown->setVisible(true);
    }
    else
    {
        _ui->_pushButtonPillarUp->hide();
        _ui->_pushButtonPillarDown->hide();
        _ui->_pushButtonDerrickUp->hide();
        _ui->_pushButtonDerrickDown->hide();
        _ui->_pushButtonOutriggerUp->hide();
        _ui->_pushButtonOutriggerDown->hide();
        _ui->_pushButtonTelBoomUp->hide();
        _ui->_pushButtonTelBoomDown->hide();
        _ui->_pushButtonHookUp->hide();
        _ui->_pushButtonHookDown->hide();

        _ui->_verticalSliderPillar->setVisible(true);
        _ui->_verticalSliderDerrick->setVisible(true);
        _ui->_verticalSliderOutrigger->setVisible(true);
        _ui->_verticalSliderTelBoom->setVisible(true);
        _ui->_verticalSliderHook->setVisible(true);
    }
}

void BluetoothClient::hidePillarControls()
{
    _ui->_pushButtonCrutchesOrPillar->setIcon(QIcon(":/pics/crutches.svg"));
    _ui->_pushButtonDerrickLabel->hide();
    _ui->_pushButtonOutriggerLabel->hide();
    _ui->_pushButtonTelBoomLabel->hide();
    if(_controls==Buttons)
    {
        _ui->_verticalSliderPillar->hide();
        _ui->_verticalSliderDerrick->hide();
        _ui->_verticalSliderOutrigger->hide();
        _ui->_verticalSliderTelBoom->hide();
        _ui->_verticalSliderHook->hide();

        _ui->_pushButtonPillarUp->setIcon(QIcon(":/pics/up.svg"));
        _ui->_pushButtonPillarDown->setIcon(QIcon(":/pics/down.svg"));
        _ui->_pushButtonDerrickUp->hide();
        _ui->_pushButtonDerrickDown->hide();
        _ui->_pushButtonOutriggerUp->hide();
        _ui->_pushButtonOutriggerDown->hide();
        _ui->_pushButtonTelBoomUp->hide();
        _ui->_pushButtonTelBoomDown->hide();

        _ui->_pushButtonPillarUp->setVisible(true);
        _ui->_pushButtonPillarDown->setVisible(true);
        _ui->_pushButtonHookUp->setVisible(true);
        _ui->_pushButtonHookDown->setVisible(true);
    }
    else
    {
        _ui->_pushButtonPillarUp->hide();
        _ui->_pushButtonPillarDown->hide();
        _ui->_pushButtonDerrickUp->hide();
        _ui->_pushButtonDerrickDown->hide();
        _ui->_pushButtonOutriggerUp->hide();
        _ui->_pushButtonOutriggerDown->hide();
        _ui->_pushButtonTelBoomUp->hide();
        _ui->_pushButtonTelBoomDown->hide();
        _ui->_pushButtonHookUp->hide();
        _ui->_pushButtonHookDown->hide();

        _ui->_verticalSliderPillar->setVisible(true);
        _ui->_verticalSliderDerrick->hide();
        _ui->_verticalSliderOutrigger->hide();
        _ui->_verticalSliderTelBoom->hide();
        _ui->_verticalSliderHook->setVisible(true);
    }
}

void BluetoothClient::setEnabledControls(bool b)
{
    _ui->_pushButtonLight->setEnabled(b);
    _ui->_pushButtonSpeed->setEnabled(b);
    _ui->_pushButtonSoundSignal->setEnabled(b);
    _ui->_pushButtonCrutchesOrPillar->setEnabled(b);
    if(_controls==Buttons)
    {
        _ui->_pushButtonPillarUp->setEnabled(b);
        _ui->_pushButtonPillarDown->setEnabled(b);
        _ui->_pushButtonDerrickUp->setEnabled(b);
        _ui->_pushButtonDerrickDown->setEnabled(b);
        _ui->_pushButtonOutriggerUp->setEnabled(b);
        _ui->_pushButtonOutriggerDown->setEnabled(b);
        _ui->_pushButtonTelBoomUp->setEnabled(b);
        _ui->_pushButtonTelBoomDown->setEnabled(b);
        _ui->_pushButtonHookUp->setEnabled(b);
        _ui->_pushButtonHookDown->setEnabled(b);
    }
    else
    {
        _ui->_verticalSliderPillar->setEnabled(b);
        _ui->_verticalSliderDerrick->setEnabled(b);
        _ui->_verticalSliderOutrigger->setEnabled(b);
        _ui->_verticalSliderTelBoom->setEnabled(b);
        _ui->_verticalSliderHook->setEnabled(b);
    }
}

void BluetoothClient::on__pushButtonPillarUp_pressed()
{
    if(_currentMode==Pillar)
    {
        moveElement(pillarUp, 0x01);
    }
    else
        moveElement(leftCrutchUp, 0x01);
}

void BluetoothClient::on__pushButtonPillarUp_released()
{
    if(_currentMode==Pillar)
        moveElement(pillarUp, 0x00);
    else
        moveElement(leftCrutchUp, 0x00);
}

void BluetoothClient::on__pushButtonPillarDown_pressed()
{
    if(_currentMode==Pillar)
        moveElement(pillarDown, 0x01);
    else
        moveElement(leftCrutchDown, 0x01);
}

void BluetoothClient::on__pushButtonPillarDown_released()
{
    if(_currentMode==Pillar)
        moveElement(pillarDown, 0x00);
    else
        moveElement(leftCrutchDown, 0x00);
}

void BluetoothClient::on__pushButtonHookUp_pressed()
{
    if(_currentMode==Pillar)
        moveElement(hookUp, 0x01);
    else
        moveElement(rightCrutchUp, 0x01);
}

void BluetoothClient::on__pushButtonHookUp_released()
{
    if(_currentMode==Pillar)
        moveElement(hookUp, 0x00);
    else
        moveElement(rightCrutchUp, 0x00);
}

void BluetoothClient::on__pushButtonHookDown_pressed()
{
    if(_currentMode==Pillar)
        moveElement(hookDown, 0x01);
    else
        moveElement(rightCrutchDown, 0x01);
}

void BluetoothClient::on__pushButtonHookDown_released()
{
    if(_currentMode==Pillar)
        moveElement(hookDown, 0x00);
    else
        moveElement(rightCrutchDown, 0x00);
}

void BluetoothClient::on__pushButtonDerrickUp_pressed()
{
    moveElement(derrickUp, 0x01);
}

void BluetoothClient::on__pushButtonDerrickUp_released()
{
    moveElement(derrickUp, 0x00);
}

void BluetoothClient::on__pushButtonDerrickDown_pressed()
{
    moveElement(derrickDown, 0x01);
}

void BluetoothClient::on__pushButtonDerrickDown_released()
{
    moveElement(derrickDown, 0x00);
}

void BluetoothClient::on__pushButtonOutriggerUp_pressed()
{
    moveElement(outriggerUp, 0x01);
}

void BluetoothClient::on__pushButtonOutriggerUp_released()
{
    moveElement(outriggerUp, 0x00);
}

void BluetoothClient::on__pushButtonOutriggerDown_pressed()
{
    moveElement(outriggerDown, 0x01);
}

void BluetoothClient::on__pushButtonOutriggerDown_released()
{
    moveElement(outriggerDown, 0x00);
}

void BluetoothClient::on__pushButtonTelBoomUp_pressed()
{
    moveElement(telescopicUp, 0x01);
}

void BluetoothClient::on__pushButtonTelBoomUp_released()
{
    moveElement(telescopicUp, 0x00);
}

void BluetoothClient::on__pushButtonTelBoomDown_pressed()
{
    moveElement(telescopicDown, 0x01);
}

void BluetoothClient::on__pushButtonTelBoomDown_released()
{
    moveElement(telescopicDown, 0x00);
}

void BluetoothClient::slotSliderAfterPressed()
{
    _localSliderChanged=false;
}

void BluetoothClient::slotSliderAfterValueChanged(int value)
{
    if(_localSliderChanged) //если значение слайдера было изменено из программы (кода)
        return;

    if(value==0)
    {
        moveElement(_currentMessage.element, 0x00);
        return;
    }

    QSlider *slider = dynamic_cast<QSlider *>(sender());

    QString nameSlider = slider->objectName();
    QString nameElement;
    if(nameSlider.indexOf("Pillar")>=0)
    {
        if(_currentMode==Pillar)
            nameElement="Pillar";
        else
            nameElement="leftCrutch";
    }
    else if(nameSlider.indexOf("Derrick")>=0)
        nameElement="Derrick";
    else if(nameSlider.indexOf("Outrigger")>=0)
        nameElement="Outrigger";
    else if(nameSlider.indexOf("TelBoom")>=0)
        nameElement="TelBoom";
    else if(nameSlider.indexOf("Hook")>=0)
    {
        if(_currentMode==Pillar)
            nameElement="Hook";
        else
            nameElement="rightCrutch";
    }

    QString suffix = (value>0)? "Up": "Down";
    nameElement+=suffix;
    _currentMessage.element = Converting::convertStringToElement(nameElement);
    _currentMessage.message = qAbs(value);
//    qDebug() << Converting::convertElementToString(_currentMessage.element) << _currentMessage.message;
    moveElement(_currentMessage.element, _currentMessage.message);
}

void BluetoothClient::slotSliderAfterReleased()
{
    _localSliderChanged=true;
    QSlider *slider = dynamic_cast<QSlider *>(sender());
    backwardSliderAfterReleased(slider);
    moveElement(_currentMessage.element, 0x00);
}

void BluetoothClient::backwardSliderAfterReleased(QSlider *slider)
{
    int v = slider->value();
    if(v==0)
        return;
    if(v>0)
    {
        while(v>0)
        {
            addValueToSlider(slider, -1);
            v--;
        }
        addValueToSlider(slider, -1);
        addValueToSlider(slider, 1);
    }
    else
    {
        while(v<0)
        {
            addValueToSlider(slider, 1);
            v++;
        }
        addValueToSlider(slider, 1);
        addValueToSlider(slider, -1);
    }
}

void BluetoothClient::addValueToSlider(QSlider *slider, int addValue)
{
    int currentValue = slider->value();
    QTime time;
    time.start();
    for(;time.elapsed() < IntervalSliderInterval;) {
        qApp->processEvents();
    }
    slider->setValue(currentValue + addValue);
}

void BluetoothClient::on__pushButtonConnecting_clicked()
{
    emit showFindDevices();
}
