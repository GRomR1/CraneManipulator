#ifndef FINDDEVICES_H
#define FINDDEVICES_H

#include <QWidget>
#include <QtWidgets/QListWidget>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qtimer.h>
#include <qdebug.h>
#include <qbluetoothsocket.h>
#include "..\Defines.h"

namespace Ui {
class FindDevices;
}

class FindDevices : public QWidget
{
    Q_OBJECT
public:
    explicit FindDevices(QWidget *parent = 0);
    ~FindDevices();
signals:
    void addressSelected(QBluetoothAddress);
    void localDeviceInfoReaded(QString name, QString address);

private slots:
    void on__pushButtonDiscovery_clicked();
    void modifyValueProgressBar();
    void itemActivated(QListWidgetItem *item);
    void deviceDiscovered(const QBluetoothDeviceInfo &device);//read information about the found devices
    void discoverFinished();

private:
    Ui::FindDevices                     *_ui;
    QBluetoothDeviceDiscoveryAgent      *_discoveryAgent;
    QList<QBluetoothDeviceInfo>          _discoveredDevices;
    bool                                 _addProgress;
    QTimer                              *_timer;
    QString                              _localName;                     ///< Имя bluetooth-клиента
    QString                              _localAddress;                  ///< Адрес bluetooth-клиента
};

#endif // FINDDEVICES_H
