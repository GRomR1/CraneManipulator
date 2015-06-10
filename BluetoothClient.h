#ifndef BLUETOOTHCLIENT_H
#define BLUETOOTHCLIENT_H

#include <QWidget>
#include <QSlider>
#include <QBluetoothUuid>
#include <QBluetoothSocket>
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
    /** \brief Конструктор
     */
    explicit BluetoothClient(QWidget *parent = 0);

    /** \brief Деструктор
     */
    ~BluetoothClient();

signals:
    /** \brief Сигнал показа окна поиска устройств
     */
    void showFindDevices();

protected:
    /** \brief Событие срабатывания таймера
     */
    virtual void timerEvent(QTimerEvent *event);

public slots:
    /** \brief Установка данных выбранного сервера и дальнейшее подключение к нему
     *  \param[in]  sName   Имя сервера
     *  \param[in]  addr    Адрес сервера
     */
    void setAddress(QString sName, QBluetoothAddress addr);

    /** \brief Установка данных выбранного сервера и дальнейшее подключение к нему
     *  \param[in]  addr    Адрес сервера
     */
    inline void setAddress(QString addr)
    {
        setAddress(addr, QBluetoothAddress(addr));
    }

    /** \brief Установка данных о себе (клиенте)
     *  \param[in]  name    Имя клиента
     *  \param[in]  addr    Адрес клиента
     */
    void setLocalNameAndAddress(QString name, QString addr);

    /** \brief Установка режима симуляции (без отправки данных в сеть)
     *  \param[in]  b       Включить/выключить режим симуляции (true/false)
     */
    void setSimulationMode(bool b);

    /** \brief Выбор рычагов или кнопок в качестве основных управляющих элементов
     *  \param[in]  c       Рычаги(QSlider)/Кнопки(QPushButton)
     */
    void setControls(Controls c);

private slots:
    /** \brief Обрабочик успешного подключения к серверу
     */
    void connected();

    /** \brief Обработчик отключения от серверу
     */
    void disconnected();

    /** \brief Обработчик чтения ошибок при работе bluetooth
     */
    void readError(QBluetoothSocket::SocketError err);

    /** \brief Обработчик чтения данных из сокета
     */
    void readSocket();

    /** \brief Обработчик записи данных в сокет
     *  \param[in]  arr     Передаваемые данные
     */
    void writeInSocket(QByteArray &arr);

    /** \brief Обработчик чтения сообщения (после чтения данных из сокета)
     *  \param[in]  el      Принятый элемент
     *  \param[in]  mes     Принятое сообщение от элемента
     */
    void readMessage(Element el, quint8 mes);

    /** \brief Обработчик отправки сообщения (перед записью данных в сокета)
     *  \param[in]  el      Передаваемый элемент
     *  \param[in]  mes     Передаваемое сообщение от элемента
     */
    void sendMessage(Element el, quint8 mes);

    /** \brief Обработчик нажатия у элементов перемещающих элементы КМУ
     *  \param[in]  el      Нажатый элемент
     *  \param[in]  mes     Сообщение от элемента
     */
    void moveElement(Element el, quint8 mes);

    /** \brief Обработчик установки иконок опор (режим Опоры)
     */
    void setCrutchesLabels();

    /** \brief Обработчик установки иконок стойки (режим Стойка, стрела, лебедка)
     */
    void setPillarAndHookLabels();

    /** \brief Обработчик включения питания (изменения иконки на кнопке питания и активация элементов управления)
     *  \param[in]  b       Включить/выключить питание
     */
    void setPowerOn(bool b=true);

    /** \brief Обработчик включения света (изменения иконки на кнопке света)
     *  \param[in]  b       Включить/выключить свет
     */
    void setLightOn(bool b=true);

    /** \brief Обработчик включения оповещения об опасности перекрутки лебедки
     *  \param[in]  b       Включить/выключить оповещение
     */
    void setHookWarning(bool b=true);

    /** \brief Обработчик включения оповещения об опасности перегрева РЖ
     *  \param[in]  b       Включить/выключить оповещение
     */
    void setTemperatureWarning(bool b=true);

    /** \brief Обработчик нажатия на кнопку включения питания
     *  \param[in]  checked    Включить(true)/выключить питание (нажатие/отжатие кнопки)
     */
    void on__pushButtonPower_clicked(bool checked);

    /** \brief Обработчик нажатия на кнопку гудка
     */
    void on__pushButtonSoundSignal_pressed();

    /** \brief Обработчик отжатия на кнопки гудка
     */
    void on__pushButtonSoundSignal_released();

    /** \brief Обработчик нажатия на кнопку включения света
     *  \param[in]  checked    Включить(true)/выключить свет (нажатие/отжатие кнопки)
     */
    void on__pushButtonLight_clicked(bool checked);

    /** \brief Обработчик нажатия на кнопку изменения скорость
     *  \param[in]  checked    Быстро(true)/медленно (нажатие/отжатие кнопки)
     */
    void on__pushButtonSpeed_clicked(bool checked);

    /** \brief Обработчик нажатия на кнопку изменения режима (опоры/стойка)
     *  \param[in]  checked    Стойка, стрела, лебедка(true)/опоры (нажатие/отжатие кнопки)
     */
    void on__pushButtonCrutchesOrPillar_clicked(bool checked);

    /** \brief Обработчик нажатия на кнопку "вверх стойка"
     */
    void on__pushButtonPillarUp_pressed();

    /** \brief Обработчик отжатия кнопки "вверх стойка"
     */
    void on__pushButtonPillarUp_released();

    /** \brief Обработчик нажатия на кнопку "вниз стойка"
     */
    void on__pushButtonPillarDown_pressed();

    /** \brief Обработчик отжатия кнопки "вниз стойка"
     */
    void on__pushButtonPillarDown_released();

    /** \brief Обработчик нажатия на кнопку "вверх лебедка"
     */
    void on__pushButtonHookUp_pressed();

    /** \brief Обработчик отжатия кнопки "вверх лебедка"
     */
    void on__pushButtonHookUp_released();

    /** \brief Обработчик нажатия на кнопку "вниз лебедка"
     */
    void on__pushButtonHookDown_pressed();

    /** \brief Обработчик отжатия кнопки "вниз лебедка"
     */
    void on__pushButtonHookDown_released();

    /** \brief Обработчик нажатия на кнопку "вверх подъемная стрела"
     */
    void on__pushButtonDerrickUp_pressed();

    /** \brief Обработчик отжатия кнопки "вверх подъемная стрела"
     */
    void on__pushButtonDerrickUp_released();

    /** \brief Обработчик нажатия на кнопку "вниз подъемная стрела"
     */
    void on__pushButtonDerrickDown_pressed();

    /** \brief Обработчик отжатия кнопки "вниз подъемная стрела"
     */
    void on__pushButtonDerrickDown_released();

    /** \brief Обработчик нажатия на кнопку "вверх выносная стрела"
     */
    void on__pushButtonOutriggerUp_pressed();

    /** \brief Обработчик отжатия кнопки "вверх выносная стрела"
     */
    void on__pushButtonOutriggerUp_released();

    /** \brief Обработчик нажатия на кнопку "вниз выносная стрела"
     */
    void on__pushButtonOutriggerDown_pressed();

    /** \brief Обработчик отжатия кнопки "вниз выносная стрела"
     */
    void on__pushButtonOutriggerDown_released();

    /** \brief Обработчик нажатия на кнопку "вверх телескопическая стрела"
     */
    void on__pushButtonTelBoomUp_pressed();

    /** \brief Обработчик отжатия кнопки "вверх телескопическая стрела"
     */
    void on__pushButtonTelBoomUp_released();

    /** \brief Обработчик нажатия на кнопку "вниз телескопическая стрела"
     */
    void on__pushButtonTelBoomDown_pressed();

    /** \brief Обработчик отжатия кнопки "вниз телескопическая стрела"
     */
    void on__pushButtonTelBoomDown_released();

    /** \brief Обработчик сигнала нажатия слайдера
     */
    void slotSliderAfterPressed();

    /** \brief Обработчик изменения значения слайдера
     *  \param[in]  value   Новое значение слайдера
     */
    void slotSliderAfterValueChanged(int value);

    /** \brief Обработчик сигнала отжатия слайдера (переход в 0)
     */
    void slotSliderAfterReleased();

    /** \brief Обработчик нажатия на имя устройства к которому произведено подключение
     * (красная надпись - правый верхний угол)
     */
    void on__pushButtonConnecting_clicked();

    /** \brief Показ надписи с ожиданием при подключении к серверу sName
     *  \param[in]  sName   Имя сервера, к которому собираемся подключиться
     */
    void showWaitingLabel(const QString &sName);

    /** \brief Показ надписи при успешном подключении к серверу sName
     *  \param[in]  sName   Имя сервера, к которому удалось подключиться
     */
    void showConnectedLabel(const QString &sName);

    /** \brief Показ надписи о отключенном сервере (о отсутсвии подлючения)
     */
    void showNotConnectedLabel();

    /** \brief Показ надписи о режиме работы "Симуляция" (без отправки данных в сеть)
     */
    void showSimulationLabel();

    /** \brief Показ надписи о режиме работы "Симуляция" (без отправки данных в сеть)
     */
    void showErrorLabel();

private:
    Ui::BluetoothClient     *_ui;                            ///< Форма (GUI)
    QBluetoothSocket        *_socket;                        ///< Основной элемент через который происходит весь обмен данными
    QBluetoothUuid           _buuid;                         ///< ID сервиса по которому происходит blutooth передача (по умолчанию - SPP)
    Mode                     _currentMode;                   ///< Текущий режим (опоры/стойка)
    int                      _currentSpeedTimeout;           ///< Текущая скорость (промежуток времени) через который отправляюется сигналы в сеть от рычагов
    int                      _timerIdCrutchesAndPillar;      ///< ID таймера для мигания иконок стойки и опор
    int                      _timerIdDerricAndTelBoom;       ///< ID таймера для мигания иконок подъемной и телескопической стрелы
    int                      _timerIdOutrigger;              ///< ID таймера для мигания иконки выносной стрелы
    int                      _timerIdErrorMessage;           ///< ID таймера для показа сообщения об ошибке bluetooth подключения/передачи
    int                      _timerIdHookWarning;            ///< ID таймера для мигания иконки концевого датчика лебедки
    int                      _timerIdTemperatureWarning;     ///< ID таймера для мигания иконки датчика перегрева РЖ
    bool                     _crutchOn;                      ///< Используется для мигания центральной иконки опор
    bool                     _pillarOn;                      ///< Используется для мигания центральной иконки стойки
    bool                     _hookOn;                        ///< Используется для мигания центральной иконки лебедки
    bool                     _derricOn;                      ///< Используется для мигания центральной иконки подъемной стрелы
    bool                     _telBoomOn;                     ///< Используется для мигания центральной иконки телескопической стрелы
    bool                     _outriggerOn;                   ///< Используется для мигания центральной иконки выносной стрелы
    bool                     _hookWarningOn;                 ///< Имеется опасность перекрутки лебедки (сработал датчик лебедки)
    bool                     _temperatureWarningOn;          ///< Имеется опасность перегрева (сработал датчик перегрева РЖ)
    Message                  _currentMessage;                ///< Последнее посланное сообщение (изменяется после изменения значения слайдера)

    QByteArray               _arr;                           ///< Текущие принятые данные (2 байта)
    QMap<int, Message>       _mapTimerIdMessages;            ///< id таймеров нажатых кнопок
    QString                  _localName;                     ///< Имя bluetooth-клиента
    QString                  _localAddress;                  ///< Адрес bluetooth-клиента
    bool                     _localSliderChanged;            ///< Значение слайдера было изменено внутри программы
    int                      _timerIdSliderControls;         ///< Служит для обработки долгого удержания слайдера в одном положении
    bool                     _simulation;                    ///< Флаг отвечающий за отправку сигналов к серверу
    Controls                 _controls;                      ///< Показываем кнопки или слайдеры

    /** \brief Показ элементов управления стойкой, стрелой, лебедкой
     */
    void showPillarControls();

    /** \brief Cкрытие элементов управления стойкой, стрелой, лебедкой
     */
    void hidePillarControls();

    /** \brief Включение/отключение всех кнопок управления краном (кроме питания)
     *  \param[in]  b           Включение/отключение (true/false)
     */
    void setEnabledControls(bool b);

    /** \brief Функция измения значения слайдера на заданную величину,
     *         через итервал IntervalSliderInterval
     *  \param[in]  slider      Изменяемый слайдер
     *  \param[in]  addValue    Добавляемая величина к слайдеру
     */
    void addValueToSlider(QSlider *slider, int addValue);

    /** \brief Возвращает слайдер в начальное значение (0)
     */
    void backwardSliderAfterReleased(QSlider *slider);
};

#endif // BLUETOOTHCLIENT_H
