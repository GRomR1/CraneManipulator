#ifndef FINDDEVICES_H
#define FINDDEVICES_H

#include <QWidget>
#include <QtWidgets/QListWidget>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qtimer.h>
#include <qbluetoothsocket.h>
#include "..\Defines.h"
#include "SavedOptionsInterface.h"

namespace Ui {
class FindDevices;
}

class FindDevices : public QWidget
{
    Q_OBJECT
public:
    explicit FindDevices(SavedOptionsInterface *options, QWidget *parent = 0);
    ~FindDevices();
signals:
    void addressSelected(QBluetoothAddress);
    void localDeviceInfoReaded(QString name, QString address);
    void selectedControls(Controls controls);
    void simulationMode(bool b); //true - is On

public slots:
    void show();

private slots:
    void on__pushButtonDiscovery_clicked();
    void modifyValueProgressBar();
    void itemActivated(QListWidgetItem *item);
    void deviceDiscovered(const QBluetoothDeviceInfo &device);//read information about the found devices
    void discoverFinished();
    void on__checkBoxSimulate_stateChanged(int arg1);

private:
    Ui::FindDevices                     *_ui;
    SavedOptionsInterface               *_options;
    QBluetoothDeviceDiscoveryAgent      *_discoveryAgent;
    QList<QBluetoothDeviceInfo>          _discoveredDevices;
    bool                                 _addProgress;
    QTimer                              *_timer;
    QString                              _localName;                     ///< Имя bluetooth-клиента
    QString                              _localAddress;                  ///< Адрес bluetooth-клиента
};

#endif // FINDDEVICES_H
