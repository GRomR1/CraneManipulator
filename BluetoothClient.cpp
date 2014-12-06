#include "BluetoothClient.h"
#include "ui_BluetoothClient.h"
#include <QFile>

BluetoothClient::BluetoothClient(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::BluetoothClient),
    _currentMode(Crutches),
    _crutchOn(true),
    _pillarOn(true),
    _hookOn(true),
    _derricOn(true),
    _telBoomOn(true),
    _outriggerOn(true)
{
    _ui->setupUi(this);
    _ui->_labelConnecting->setText("<FONT COLOR = RED>Not connected.</FONT>");
    _ui->_labelConnecting->setStyleSheet("color: red");

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

    _ui->_pushButtonPillarUp->setIconSize(_ui->_pushButtonPillarUp->size()-QSize(80,80));
    _ui->_pushButtonPillarDown->setIconSize(_ui->_pushButtonPillarDown->size()-QSize(80,80));
    _ui->_pushButtonDerrickUp->setIconSize(_ui->_pushButtonDerrickUp->size()-QSize(80,80));
    _ui->_pushButtonDerrickDown->setIconSize(_ui->_pushButtonDerrickDown->size()-QSize(80,80));
    _ui->_pushButtonOutriggerUp->setIconSize(_ui->_pushButtonOutriggerUp->size()-QSize(80,80));
    _ui->_pushButtonOutriggerDown->setIconSize(_ui->_pushButtonOutriggerDown->size()-QSize(80,80));
    _ui->_pushButtonTelBoomUp->setIconSize(_ui->_pushButtonTelBoomUp->size()-QSize(80,80));
    _ui->_pushButtonTelBoomDown->setIconSize(_ui->_pushButtonTelBoomDown->size()-QSize(80,80));
    _ui->_pushButtonHookUp->setIconSize(_ui->_pushButtonHookUp->size()-QSize(80,80));
    _ui->_pushButtonHookDown->setIconSize(_ui->_pushButtonHookDown->size()-QSize(80,80));

    QFile file(":/simple.qss");
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    qApp->setStyleSheet(strCSS);

    _ui->_pushButtonDerrickLabel->hide();
    _ui->_pushButtonOutriggerLabel->hide();
    _ui->_pushButtonTelBoomLabel->hide();
    _ui->_pushButtonDerrickUp->hide();
    _ui->_pushButtonDerrickDown->hide();
    _ui->_pushButtonOutriggerUp->hide();
    _ui->_pushButtonOutriggerDown->hide();
    _ui->_pushButtonTelBoomUp->hide();
    _ui->_pushButtonTelBoomDown->hide();

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
    delete _ui;
}

void BluetoothClient::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == _timerIdCrutchesAndPillar)
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
    if(event->timerId() == _timerIdDerricAndTelBoom)
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

    if(event->timerId() == _timerIdOutrigger)
    {
        if(_outriggerOn)
            _ui->_pushButtonOutriggerLabel->setIcon(QIcon(":/pics/outrigger_black.svg"));
        else
            _ui->_pushButtonOutriggerLabel->setIcon(QIcon(":/pics/outrigger_white.svg"));
        _outriggerOn=!_outriggerOn;
    }
    if(event->timerId() == _timerIdErrorMessage)
    {
        killTimer(_timerIdErrorMessage);
        _ui->_labelError->clear();
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
//    qDebug()<<label;
    _ui->_labelConnecting->setText("<FONT COLOR = GREEN>"+label+"</FONT>");
}

void BluetoothClient::disconnected()
{
    QString label = "Server " + _socket->peerName()+ " disconnected";
//    qDebug()<<label;
    _ui->_labelConnecting->setText("<FONT COLOR = RED>"+label+"</FONT>");
}

void BluetoothClient::readSocket()
{
    if (!_socket)
        return;

    QByteArray arr;
    arr = _socket->readAll();
    QDataStream in(&arr, QIODevice::ReadOnly);
    quint8 el;
    quint8 mes;
    in >> el >> mes;
    qDebug() << arr.toHex();
    readMessage((Element)el, mes );
}

void BluetoothClient::readMessage(Element el, quint8 mes)
{
    qDebug() << (quint8)el << mes;
}

void BluetoothClient::sendMessage(Element el, quint8 mes)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);

    out << quint8(el) << mes;
    qDebug() << arrBlock.toHex();
#ifndef LOCAL_SIMULATE
    _socket->write(arrBlock);
#endif
}


void BluetoothClient::setAddress(QBluetoothAddress addr)
{
#ifndef LOCAL_SIMULATE
    _socket->connectToService(addr, _buuid);
#endif
}

void BluetoothClient::setAddress(QString addr)
{
#ifndef LOCAL_SIMULATE
    _socket->connectToService(QBluetoothAddress(addr), _buuid);
#endif
}

void BluetoothClient::on__pushButtonPower_clicked(bool checked)
{
//    qDebug() << checked;
//    checked == true - питание вкл.
    if(checked)
        sendMessage(powerButton, 0x01);
    else
        sendMessage(powerButton, 0x00);

#ifdef DEBUG
    setPowerOn(checked); // установить переключатель не дожидаясь ответа от КМУ
#endif
}

void BluetoothClient::setPowerOn(bool b)
{
    if(b)
    {
        _ui->_pushButtonPower->setStyleSheet("background-color: rgba(0, 255, 0, 10%);");
        _ui->_pushButtonPower->setIcon(QIcon(":/pics/power_green.svg"));
    }
    else
    {
        _ui->_pushButtonPower->setStyleSheet("background-color: rgba(255, 0, 0, 15%);");
        _ui->_pushButtonPower->setIcon(QIcon(":/pics/power_red.svg"));
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

#ifdef DEBUG
    setLightOn(checked); // установить переключатель недожидаясь ответа от КМУ
#endif
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

void BluetoothClient::on__pushButtonSpeed_clicked(bool checked)
{
    //    qDebug() << checked;
    //    checked == true - быстро
    if(checked)
    {
        sendMessage(speedButton, 0x01);
        _ui->_pushButtonSpeed->setIcon(QIcon(":/pics/speed_high.svg"));
    }
    else
    {
        sendMessage(speedButton, 0x00);
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
        _ui->_pushButtonCrutchesOrPillar->setIcon(QIcon(":/pics/pillar_boom_hook.svg"));
        _ui->_pushButtonPillarUp->setIcon(QIcon(":/pics/rotate_up.svg"));
        _ui->_pushButtonPillarDown->setIcon(QIcon(":/pics/rotate_down.svg"));

        _ui->_pushButtonDerrickLabel->setVisible(true);
        _ui->_pushButtonOutriggerLabel->setVisible(true);
        _ui->_pushButtonTelBoomLabel->setVisible(true);
        _ui->_pushButtonDerrickUp->setVisible(true);
        _ui->_pushButtonDerrickDown->setVisible(true);
        _ui->_pushButtonOutriggerUp->setVisible(true);
        _ui->_pushButtonOutriggerDown->setVisible(true);
        _ui->_pushButtonTelBoomUp->setVisible(true);
        _ui->_pushButtonTelBoomDown->setVisible(true);
    }
    else
    {
        _currentMode = Crutches;
        setCrutchesLabels();
        _ui->_labelCrutchesOrPillar->setText("Опоры");
        _ui->_pushButtonCrutchesOrPillar->setIcon(QIcon(":/pics/crutches.svg"));
        _ui->_pushButtonPillarUp->setIcon(QIcon(":/pics/up.svg"));
        _ui->_pushButtonPillarDown->setIcon(QIcon(":/pics/down.svg"));

        _ui->_pushButtonDerrickLabel->hide();
        _ui->_pushButtonOutriggerLabel->hide();
        _ui->_pushButtonTelBoomLabel->hide();
        _ui->_pushButtonDerrickUp->hide();
        _ui->_pushButtonDerrickDown->hide();
        _ui->_pushButtonOutriggerUp->hide();
        _ui->_pushButtonOutriggerDown->hide();
        _ui->_pushButtonTelBoomUp->hide();
        _ui->_pushButtonTelBoomDown->hide();
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

void BluetoothClient::on__pushButtonPillarUp_pressed()
{
    if(_currentMode==Pillar)
        sendMessage(pillarUp, 0x01);
    else
        sendMessage(leftCrutchUp, 0x01);
}

void BluetoothClient::on__pushButtonPillarUp_released()
{
    if(_currentMode==Pillar)
        sendMessage(pillarUp, 0x00);
    else
        sendMessage(leftCrutchUp, 0x00);
}

void BluetoothClient::on__pushButtonPillarDown_pressed()
{
    if(_currentMode==Pillar)
        sendMessage(pillarDown, 0x01);
    else
        sendMessage(leftCrutchDown, 0x01);
}

void BluetoothClient::on__pushButtonPillarDown_released()
{
    if(_currentMode==Pillar)
        sendMessage(pillarDown, 0x00);
    else
        sendMessage(leftCrutchDown, 0x00);
}

void BluetoothClient::on__pushButtonHookUp_pressed()
{
    if(_currentMode==Pillar)
        sendMessage(hookUp, 0x01);
    else
        sendMessage(rightCrutchUp, 0x01);
}

void BluetoothClient::on__pushButtonHookUp_released()
{
    if(_currentMode==Pillar)
        sendMessage(hookUp, 0x00);
    else
        sendMessage(rightCrutchUp, 0x00);
}

void BluetoothClient::on__pushButtonHookDown_pressed()
{
    if(_currentMode==Pillar)
        sendMessage(hookDown, 0x01);
    else
        sendMessage(rightCrutchDown, 0x01);
}

void BluetoothClient::on__pushButtonHookDown_released()
{
    if(_currentMode==Pillar)
        sendMessage(hookDown, 0x00);
    else
        sendMessage(rightCrutchDown, 0x00);
}

void BluetoothClient::on__pushButtonDerrickUp_pressed()
{
    sendMessage(derrickUp, 0x01);
}

void BluetoothClient::on__pushButtonDerrickUp_released()
{
    sendMessage(derrickUp, 0x00);
}

void BluetoothClient::on__pushButtonDerrickDown_pressed()
{
    sendMessage(derrickDown, 0x01);
}

void BluetoothClient::on__pushButtonDerrickDown_released()
{
    sendMessage(derrickDown, 0x00);
}

void BluetoothClient::on__pushButtonOutriggerUp_pressed()
{
    sendMessage(outriggerUp, 0x01);
}

void BluetoothClient::on__pushButtonOutriggerUp_released()
{
    sendMessage(outriggerUp, 0x00);
}

void BluetoothClient::on__pushButtonOutriggerDown_pressed()
{
    sendMessage(outriggerDown, 0x01);
}

void BluetoothClient::on__pushButtonOutriggerDown_released()
{
    sendMessage(outriggerDown, 0x00);
}

void BluetoothClient::on__pushButtonTelBoomUp_pressed()
{
    sendMessage(telescopicUp, 0x01);
}

void BluetoothClient::on__pushButtonTelBoomUp_released()
{
    sendMessage(telescopicUp, 0x00);
}

void BluetoothClient::on__pushButtonTelBoomDown_pressed()
{
    sendMessage(telescopicDown, 0x01);
}

void BluetoothClient::on__pushButtonTelBoomDown_released()
{
    sendMessage(telescopicDown, 0x00);
}
