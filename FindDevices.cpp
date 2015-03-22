#include "FindDevices.h"
#include "ui_FindDevices.h"
#include <QFile>

FindDevices::FindDevices(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::FindDevices),
    _addProgress(true)
{
    _ui->setupUi(this);

    QFile file(":/simple.qss");
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    qApp->setStyleSheet(strCSS);

    _ui->_checkBoxSimulate->hide();
    _ui->_checkBoxRemind->hide();
    _ui->_radioButtonSetButton->hide();
    _ui->_radioButtonSetSlider->hide();

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
        //emit localDeviceInfoReaded(localDevice.name(), localDevice.address().toString());

        /*
//        Make it visible to others
//        localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);

        // Get connected devices
        QList<QBluetoothAddress> remotes;
        remotes = localDevice.connectedDevices();
        foreach (QBluetoothAddress addr, remotes) {
            qDebug()<< "Connected device 0: " <<addr.toString();
        }

        QBluetoothAddress adapterAddress = localDevice.address();
        _discoveryAgentService = new QBluetoothServiceDiscoveryAgent(adapterAddress);
        connect(_discoveryAgentService, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
                SLOT(startClient(QBluetoothServiceInfo)));
//        connect(_discoveryAgentService, SIGNAL(finished()), SLOT(finishedDiscoveryService()));
        */

        on__pushButtonDiscovery_clicked();
    }
    else
    {
        _ui->_labelMyInfo->setText(trUtf8("Bluetooth модуль не обнаружен"));
        _ui->_pushButtonDiscovery->setEnabled(false);
//        close();
    }

}

FindDevices::~FindDevices()
{
    delete _ui;
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
    emit addressSelected(address);
    emit localDeviceInfoReaded(_localName, _localAddress);
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
    _timer->stop();
    _ui->_pushButtonDiscovery->setText(trUtf8("Поиск"));
    _discoveredDevices = _discoveryAgent->discoveredDevices();
    if(_ui->_listWidgetDevices->count()==0)
        _ui->_listWidgetDevices->addItem(trUtf8("Устройства не обнаружены"));
}
