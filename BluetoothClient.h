#ifndef BLUETOOTHCLIENT_H
#define BLUETOOTHCLIENT_H

#include <QWidget>
#include <QSlider>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothsocket.h>
#include "../Defines.h"

namespace Ui {
class BluetoothClient;
}

/** \brief Класс для управления КМУ по bluetooth (интерфейс + обработчики)
 */
class BluetoothClient : public QWidget
{
    Q_OBJECT

public:
    explicit BluetoothClient(QWidget *parent = 0);
    ~BluetoothClient();

signals:
    /** \brief Сигнал показа окна поиска устройств
     */
    void showFindDevices();

protected:
    virtual void timerEvent(QTimerEvent *event);

public slots:
    void setAddress(QBluetoothAddress addr);
    void setAddress(QString addr);
    void setLocalNameAndAddress(QString name, QString addr);
    void setSimulationMode(bool b);
    void setControls(Controls c);

private slots:
    void connected();
    void disconnected();
    void readError(QBluetoothSocket::SocketError err);
    void readSocket();
    void writeInSocket(QByteArray &arr);
    void readMessage(Element el, quint8 mes);
    void sendMessage(Element el, quint8 mes);

    /** \brief Обработка нажатия у элементов, которые перемещают  кран (кнопки со стрелками)
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

    /** \brief Обработчик сигнала нажатия слайдера
     */
    void slotSliderAfterPressed();

    /** \brief Обработчик изменения значения слайдера
     */
    void slotSliderAfterValueChanged(int value);

    /** \brief Обработчик сигнала отжатия слайдера (переход в 0)
     */
    void slotSliderAfterReleased();

    /** \brief Обработчик нажатия на имя устройства к которому произведено подключение
     * (красная надпись - правый верхний угол)
     */
    void on__pushButtonConnecting_clicked();

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
    Message                  _currentMessage;                ///< Последнее посланное сообщение (изменяется после изменения значения слайдера)

    QByteArray               _arr;                           ///< Текущие принятые данные (2 байта)
    QMap<int, Message>       _mapTimerIdMessages;            ///< id таймеров нажатых кнопок
    QString                  _localName;                     ///< Имя bluetooth-клиента
    QString                  _localAddress;                  ///< Адрес bluetooth-клиента
    bool                     _localSliderChanged;            ///< Значение слайдера было изменено внутри программы
    int                      _timerIdSliderControls;         ///< Служит для обработки долгого удержания слайдера в одном положении
    bool                     _simulation;                    ///< Флаг отвечающий за отправку сигналов к серверу
    Controls                 _controls;                      ///< Показываем кнопки или слайдеры

    /** \brief Показ/скрытие элементов управления стойкой, стрелой, лебедкой
     */
    void showPillarControls();
    void hidePillarControls();

    /** \brief Включение/отключение (true/false) всех кнопок управления краном
     */
    void setEnabledControls(bool b);

    /** \brief Функция измения значения слайдера на заданную величину,
     *         через итервал IntervalSliderInterval
     */
    void addValueToSlider(QSlider *slider, int addValue);

    /** \brief Возвращает слайдер в начальное значение (0)
     */
    void backwardSliderAfterReleased(QSlider *slider);
};

#endif // BLUETOOTHCLIENT_H
