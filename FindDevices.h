#ifndef FINDDEVICES_H
#define FINDDEVICES_H

#include <QWidget>
#include <QtWidgets/QListWidget>
#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QTimer>
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
    /** \brief Конструктор
     *  \param[in]  options   Указатель на объект содержащий сохраненные настройки
     *  \param[in]  parent    Родитель в иерахии Qt
     */
    explicit FindDevices(SavedOptionsInterface *options, QWidget *parent = 0);

    /** \brief Деструктор
     */
    ~FindDevices();

signals:
    /** \brief Сигнал вызываемый при выборе записи о сервере для подключения.
     *
     * Передает имя и mac-адрес сервера
     */
    void itemSelected(QString serverName, QBluetoothAddress serverAddress);

    /** \brief Сигнал вызываемый при выборе записи о сервере для подключения, передающий данные о себе
     *
     * Передает имя и mac-адрес клиента (свои)
     */
    void localDeviceInfoReaded(QString name, QString address);

    /** \brief Сигнал вызываемый при выборе записи о сервере для подключения или симуляции
     *
     * Передает тип выбранных управляющих элементов (кнопки или рычаги)
     */
    void selectedControls(Controls controls);


    /** \brief Сигнал вызываемый подключении в режиме симуляции
     *
     * True - хочу подключится в режиме симуляции
     */
    void simulationMode(bool b);

    /** \brief Сигнал вызываемый для показа окна объекта BluetoothClient
     */
    void showBluetoothClient();

public slots:
    void show();                                                ///< Отображение виджета

private slots:
    void modifyValueProgressBar();                              ///< Изменить значение ProgressBar на 1 (+1 или -1) в зависимости от флага _addProgress

    /** \brief Обработчик выбора элемента в списке найденных устройств
     *  \param[in]  item     Указатель на объект содержащий выбранный элемент
     */
    void itemActivated(QListWidgetItem *item);

    /** \brief Обработчик читающий информацию о найденном BT-устройстве
     *  \param[in]  device   Найденное устройство
     */
    void deviceDiscovered(const QBluetoothDeviceInfo &device);

    void discoverFinished();                                    ///< Вызывается после окончания поиска
    void on__pushButtonDiscovery_clicked();                     ///< Начать поиск активных устройств (нажатие на кнопку Поиск)

    /** \brief Обработчик изменения состояния чекбокса Симуляция
     *  \param[in]  arg1     Новое состояние
     */
    void on__checkBoxSimulate_stateChanged(int arg1);

private:
    Ui::FindDevices                     *_ui;                   ///< Форма (GUI)
    SavedOptionsInterface               *_options;              ///< Сохраненные опции
    QBluetoothDeviceDiscoveryAgent      *_discoveryAgent;       ///< Агент поиска bluetooth-устройств
    QList<QBluetoothDeviceInfo>          _discoveredDevices;    ///< Найденные устройства
    bool                                 _addProgress;          ///< Флаг увеличения или уменьшения ProgressBar'а
    QTimer                              *_timer;                ///< Используется для полосы поиска устройств (визуализации ProgressBar)
    QString                              _localName;            ///< Имя bluetooth-клиента
    QString                              _localAddress;         ///< Адрес bluetooth-клиента
};

#endif // FINDDEVICES_H
