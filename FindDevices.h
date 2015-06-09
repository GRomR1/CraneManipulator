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

/** \brief Класс для поиска bluetooth-приемников и выбора подходящего
 */
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
    void showBluetoothClient();

public slots:
    void show();

private slots:
    void modifyValueProgressBar();                              /// Изменить значение ProgressBar на 1 (+1 или -1)
    void itemActivated(QListWidgetItem *item);                  /// Срабатыватывает по выбору строки в таблице найденных устройств
    void deviceDiscovered(const QBluetoothDeviceInfo &device);  /// Чтение информации о найденном устройстве
    void discoverFinished();                                    /// Вызывается после окончания поиска
    void on__pushButtonDiscovery_clicked();                     /// Начать поиск активных устройств (кнопка Поиск)
    void on__checkBoxSimulate_stateChanged(int arg1);           /// Вкл/выкл режим симуляции (чекбокс Симуляция)

private:
    Ui::FindDevices                     *_ui;
    SavedOptionsInterface               *_options;              /// Сохраненные опции
    QBluetoothDeviceDiscoveryAgent      *_discoveryAgent;       /// Агент поиска bluetooth-устройств
    QList<QBluetoothDeviceInfo>          _discoveredDevices;    /// Найденные устройства
    bool                                 _addProgress;          /// Флаг увеличения или уменьшения ProgressBar'а
    QTimer                              *_timer;                /// Используется для полосы поиска устройств (визуализации ProgressBar)
    QString                              _localName;            /// Имя bluetooth-клиента
    QString                              _localAddress;         /// Адрес bluetooth-клиента
};

#endif // FINDDEVICES_H
