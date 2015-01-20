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
        _ui->_labelMyInfo->setText("My bluetooth name: "+localDevice.name()+"\n"
                                  +"My address: "+localDevice.address().toString());

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

    }
    else
    {
        _ui->_labelMyInfo->setText("Bluetooth adapters not found");
        _ui->_pushButtonDiscovery->setEnabled(false);
//        close();
    }

    on__pushButtonDiscovery_clicked();
}

FindDevices::~FindDevices()
{
    delete _ui;
}

void FindDevices::on__pushButtonDiscovery_clicked()
{
    if(_ui->_pushButtonDiscovery->text()=="Discovery")
    {
        _ui->_progressBar->reset();
        _ui->_listWidgetDevices->clear();
        _timer->start(25);
        if(_discoveryAgent->isActive())
            _discoveryAgent->stop();
        _discoveryAgent->start();   // Start a discovery
        _ui->_pushButtonDiscovery->setText("Stop");
        return;
    }

    if(_ui->_pushButtonDiscovery->text()=="Stop")
    {
        if(_discoveryAgent->isActive())
            _discoveryAgent->stop();
        discoverFinished();
        _ui->_pushButtonDiscovery->setText("Discovery");
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
    if(text == "Devices not found")
        return;
    int index = text.indexOf('(');
    if (index == -1)
        return;

    if(_discoveryAgent->isActive())
        _discoveryAgent->stop();
    QString sName = text;
    sName.remove(index, text.size()-index);
    QString sAddress = text;
    sAddress.remove(0, text.indexOf('(')+1).remove(')');
    QBluetoothAddress address(sAddress);

//    qDebug() << sName << sAddress;
    emit addressSelected(address);
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
    _ui->_pushButtonDiscovery->setText("Discovery");
    _discoveredDevices = _discoveryAgent->discoveredDevices();
    if(_ui->_listWidgetDevices->count()==0)
        _ui->_listWidgetDevices->addItem("Devices not found");
}
