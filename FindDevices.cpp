#include "FindDevices.h"
#include "ui_FindDevices.h"
#include <QBluetoothLocalDevice>

FindDevices::FindDevices(SavedOptionsInterface *options, QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::FindDevices),
    _options(options),
    _discoveryAgent(0),
    _addProgress(true),
    _timer(0)
{
    _ui->setupUi(this);

//    _ui->_checkBoxSimulate->hide();
//    _ui->_checkBoxRemind->hide();
//    _ui->_radioButtonSetButton->hide();
//    _ui->_radioButtonSetSlider->hide();

    QBluetoothLocalDevice localDevice;

    _discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(_discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
             SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    connect(_discoveryAgent, SIGNAL(finished()), SLOT(discoverFinished()));

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), SLOT(modifyValueProgressBar()));

    connect(_ui->_listWidgetDevices, SIGNAL(itemActivated(QListWidgetItem*)),
            SLOT(itemActivated(QListWidgetItem*)));

    // Check if Bluetooth is available on this device
    if(localDevice.isValid())
    {
        // Turn Bluetooth on
        localDevice.powerOn();

        // Read local device name
        _localName = localDevice.name();
        _localAddress = localDevice.address().toString();
        _ui->_labelMyInfo->setText(trUtf8("О себе: ")+_localName+" ("+_localAddress+")");

        on__pushButtonDiscovery_clicked();
    }
    else
    {
        _ui->_labelMyInfo->setText(trUtf8("Bluetooth модуль не обнаружен"));

        if(!_ui->_checkBoxSimulate->isChecked())
            _ui->_pushButtonDiscovery->setEnabled(false);
    }

}

FindDevices::~FindDevices()
{
    delete _ui;
}

void FindDevices::show()
{
    _ui->_radioButtonSetButton->setChecked(true);

    if(_options->isSliders())
        _ui->_radioButtonSetSlider->setChecked(true);
    if(_options->keepIsMind())
        _ui->_checkBoxRemind->setChecked(true);
    if(_options->simulation())
        _ui->_checkBoxSimulate->setChecked(true);

    if(_options->isSliders())
    {
        _ui->_radioButtonSetSlider->setChecked(true);
//        qDebug() << "sliders";
    }
    else
    {
        //if(_options->isButtons())
        _ui->_radioButtonSetButton->setChecked(true);
//        qDebug() << "buttons";
    }
    if(_options->keepIsMind())
        _ui->_checkBoxRemind->setChecked(true);
    if(_options->simulation())
        _ui->_checkBoxSimulate->setChecked(true);
    QWidget::show();
}

void FindDevices::on__pushButtonDiscovery_clicked()
{
    if(_ui->_pushButtonDiscovery->text()==trUtf8("Поиск"))
    {
        _ui->_progressBar->reset();
        _ui->_listWidgetDevices->clear();
        _timer->start(25);
        if(_discoveryAgent->isActive())
            _discoveryAgent->stop();
        _discoveryAgent->start();   // Start a discovery
        _ui->_pushButtonDiscovery->setText(trUtf8("Стоп"));
        return;
    }
    if(_ui->_pushButtonDiscovery->text()==trUtf8("Стоп"))
    {
        if(_discoveryAgent->isActive())
            _discoveryAgent->stop();
        discoverFinished();
        _ui->_pushButtonDiscovery->setText(trUtf8("Поиск"));
        return;
    }
    if(_ui->_pushButtonDiscovery->text()==trUtf8("Cимуляция"))
    {
        _options->setClientInfo(ClientInfo(_localName, _localAddress));
        if(_ui->_radioButtonSetButton->isChecked())
            _options->setButtons();
        else
            _options->setSliders();
        if(_ui->_checkBoxRemind->isChecked())
            _options->setKeepIsMind(true);
        else
            _options->setKeepIsMind(false);
        _options->setSimulation(true);
        emit selectedControls(_options->controls());
        emit simulationMode(true);
        emit showBluetoothClient();
        return;
    }
}

void FindDevices::modifyValueProgressBar()
{
    int v=_ui->_progressBar->value();

    if(_addProgress)
    {
        if(v>=100)
        {
            _addProgress=false;
            _ui->_progressBar->setValue(--v);
        }
        else
            _ui->_progressBar->setValue(++v);
    }
    else
    {
        if(v<=0)
        {
            _addProgress=true;
            _ui->_progressBar->setValue(++v);
        }
        else
            _ui->_progressBar->setValue(--v);
    }
}

void FindDevices::itemActivated(QListWidgetItem *item)
{
    QString text = item->text();
    if(text == trUtf8("Устройства не обнаружены"))
        return;
    int index = text.indexOf('(');
    if (index == -1)
        return;

    if(_discoveryAgent->isActive())
        discoverFinished();
    QString sName = text;
    sName.remove(index, text.size()-index);
    QString sAddress = text;
    sAddress.remove(0, text.indexOf('(')+1).remove(')');
    QBluetoothAddress address(sAddress);

//    qDebug() << sName << sAddress;
    _options->setServerAddress(sAddress);
    _options->setServerName(sName);
    _options->setClientInfo(ClientInfo(_localName, _localAddress));
    if(_ui->_checkBoxRemind->isChecked())
        _options->setKeepIsMind(true);
    else
        _options->setKeepIsMind(false);

    if(_ui->_radioButtonSetButton->isChecked())
        _options->setButtons();
    else
        _options->setSliders();
    _options->setSimulation(false);
    emit selectedControls(_options->controls());
    emit itemSelected(sName, address);
    emit localDeviceInfoReaded(_localName, _localAddress);
    emit showBluetoothClient();
}

void FindDevices::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    QString item = device.name() + '(' + device.address().toString() + ')';
    if(!(_ui->_listWidgetDevices->findItems(item, Qt::MatchExactly)).isEmpty())
        return;
    _ui->_listWidgetDevices->addItem(item);
}

void FindDevices::discoverFinished()
{
    _ui->_progressBar->setValue(100);
    if(_timer->isActive())
        _timer->stop();
    _ui->_pushButtonDiscovery->setText(trUtf8("Поиск"));
    if(_discoveryAgent)
        _discoveredDevices = _discoveryAgent->discoveredDevices();
    if(_ui->_listWidgetDevices->count()==0)
        _ui->_listWidgetDevices->addItem(trUtf8("Устройства не обнаружены"));
}

void FindDevices::on__checkBoxSimulate_stateChanged(int arg1)
{
    if(arg1==Qt::Checked)
    {
        _ui->_progressBar->setValue(100);
        if(_timer->isActive())
            _timer->stop();
        if(_discoveryAgent->isActive())
            _discoveryAgent->stop();
        _ui->_pushButtonDiscovery->setEnabled(true);
        _ui->_pushButtonDiscovery->setText(trUtf8("Cимуляция"));
        _ui->_listWidgetDevices->clear();
    }
    else
    {
        _ui->_pushButtonDiscovery->setText(trUtf8("Поиск"));
        if(_ui->_labelMyInfo->text()==trUtf8("Bluetooth модуль не обнаружен"))
            _ui->_pushButtonDiscovery->setEnabled(false);
    }

}
