#ifndef SAVEDOPTIONSINTERFACE_H
#define SAVEDOPTIONSINTERFACE_H

#include "../Defines.h"
#include <QSettings>

/** \brief Класс для сохранения настроек пульта управления.
 *
 *  Содержит в себе следующие настройки: имя и MAC-адрес клиента, MAC-адрес сервера,
 *  включение режима симуляции (без подключения к серверу и отправки сигналов в сеть),
 *  выбор управления (кнопки/рычаги(слайдеры))
 */
class SavedOptionsInterface
{
public:
    SavedOptionsInterface();
    bool keepIsMind() const;
    void setKeepIsMind(bool b=true);
    bool simulation() const;
    void setSimulation(bool b=true);
    Controls controls() const;
    bool isButtons() const;
    bool isSliders() const;
    void setButtons();
    void setSliders();
    ClientInfo clientInfo() const;
    void setClientInfo(ClientInfo ci);
    QString serverAddress() const;
    void setServerAddress(QString &sa);
    QString serverName() const;
    void setServerName(QString &sn);

    void readOptions();
    void writeOptions();

private:
    bool        _keepInMind;
    bool        _simulation;
    QSettings   _settings;
    QString     _serverAddress;
    QString     _serverName;
    ClientInfo  _clientInfo;
    Controls    _controls;
};

#endif // SAVEDOPTIONSINTERFACE_H
