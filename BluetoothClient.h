#ifndef BLUETOOTHCLIENT_H
#define BLUETOOTHCLIENT_H

#include <QWidget>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothsocket.h>
#include <QDebug>
#include "../Defines.h"

namespace Ui {
class BluetoothClient;
}

class BluetoothClient : public QWidget
{
    Q_OBJECT

public:
    explicit BluetoothClient(QWidget *parent = 0);
    ~BluetoothClient();

protected:
    virtual void timerEvent(QTimerEvent *event);

public slots:
    void setAddress(QBluetoothAddress addr);
    void setAddress(QString addr);
    void setLocalNameAndAddress(QString name, QString addr);

private slots:
    void connected();
    void disconnected();
    void readError(QBluetoothSocket::SocketError err);
    void readSocket();
    void writeInSocket(QByteArray &arr);
    void readMessage(Element el, quint8 mes);
    void sendMessage(Element el, quint8 mes);

    /** @brief Обработка нажатия у элементов, которые перемещают  кран (кнопки со стрелками)
     */
    void moveElement(Element el, quint8 mes);

    void setCrutchesLabels();
    void setPillarAndHookLabels();

    void setPowerOn(bool b=true);
    void setLightOn(bool b=true);
    void setHookWarning(bool b=true);
    void setTemperatureWarning(bool b=true);

    void on__pushButtonPower_clicked(bool checked);
    void on__pushButtonSoundSignal_pressed();
    void on__pushButtonSoundSignal_released();
    void on__pushButtonLight_clicked(bool checked);
    void on__pushButtonSpeed_clicked(bool checked);
    void on__pushButtonCrutchesOrPillar_clicked(bool checked);
    void on__pushButtonPillarUp_pressed();
    void on__pushButtonPillarUp_released();
    void on__pushButtonPillarDown_pressed();
    void on__pushButtonPillarDown_released();
    void on__pushButtonHookUp_pressed();
    void on__pushButtonHookUp_released();
    void on__pushButtonHookDown_pressed();
    void on__pushButtonHookDown_released();
    void on__pushButtonDerrickUp_pressed();
    void on__pushButtonDerrickUp_released();
    void on__pushButtonDerrickDown_pressed();
    void on__pushButtonDerrickDown_released();
    void on__pushButtonOutriggerUp_pressed();
    void on__pushButtonOutriggerUp_released();
    void on__pushButtonOutriggerDown_pressed();
    void on__pushButtonOutriggerDown_released();
    void on__pushButtonTelBoomUp_pressed();
    void on__pushButtonTelBoomUp_released();
    void on__pushButtonTelBoomDown_pressed();
    void on__pushButtonTelBoomDown_released();

private:
    Ui::BluetoothClient     *_ui;
    QBluetoothSocket        *_socket;
    QBluetoothUuid           _buuid;
    Mode                     _currentMode;
    int                      _currentSpeedTimeout;
    int                      _timerIdCrutchesAndPillar;
    int                      _timerIdDerricAndTelBoom;
    int                      _timerIdOutrigger;
    int                      _timerIdErrorMessage;
    int                      _timerIdHookWarning;
    int                      _timerIdTemperatureWarning;
    bool                     _crutchOn;
    bool                     _pillarOn;
    bool                     _hookOn;
    bool                     _derricOn;
    bool                     _telBoomOn;
    bool                     _outriggerOn;
    bool                     _hookWarningOn;
    bool                     _temperatureWarningOn;

    QByteArray               _arr;                           ///< Хранит текущие принятые данные (2 байта)
    QMap<int, Message>       _mapTimerIdMessages;            ///< Хранит id таймеров нажатых кнопок
    QString                  _localName;                     ///< Имя bluetooth-клиента
    QString                  _localAddress;                  ///< Адрес bluetooth-клиента

};

#endif // BLUETOOTHCLIENT_H
