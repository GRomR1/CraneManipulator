#include "SavedOptionsInterface.h"

SavedOptionsInterface::SavedOptionsInterface() :
    _keepInMind(false),
    _simulation(false),
    _settings("PNRPU","CraneManipulator"),
    _controls(Sliders)
{
}

void SavedOptionsInterface::readOptions()
{
//    _settings.clear();
//    qDebug() << _settings.allKeys();
//    qDebug() << _settings.fileName();
//    _settings.beginGroup("crane_configs");
//    qDebug() << _settings.allKeys() ;
//    qDebug() << _settings.value("_controls").toString();

//    qDebug() << "_keepinmind status" << _settings.contains("_keepinmind") ;
    _keepInMind         = _settings.value("_keepinmind", false).toBool();
//    qDebug() << "_keepinmind = " << _keepInMind;
    _simulation         = _settings.value("_simulation", false).toBool();
    _serverAddress      = _settings.value("_serveraddress", "").toString();
    _clientInfo.name    = _settings.value("_clientname", "").toString();
    _clientInfo.address = _settings.value("_clientaddress", "").toString();
    _controls           = Converting::convertStringToControls(_settings.value("_controls", "Sliders").toString());
//    qDebug() << "_controls = " << _settings.value("_controls").toString();
//    qDebug() << "_controls = " << _controls;
}

void SavedOptionsInterface::writeOptions()
{
    _settings.setValue("_keepinmind", _keepInMind);
    _settings.setValue("_simulation", _simulation);
    _settings.setValue("_serveraddress", _serverAddress);
    _settings.setValue("_clientname", _clientInfo.name);
    _settings.setValue("_clientaddress", _clientInfo.address);
    _settings.setValue("_controls", Converting::convertControlsToString(_controls));

}

bool SavedOptionsInterface::keepIsMind() const
{
    return _keepInMind;
}

void SavedOptionsInterface::setKeepIsMind(bool b)
{
    _keepInMind=b;
    writeOptions();
}

bool SavedOptionsInterface::simulation() const
{
    return _simulation;
}

void SavedOptionsInterface::setSimulation(bool b)
{
    _simulation=b;
    writeOptions();
}

Controls SavedOptionsInterface::controls() const
{
    return _controls;
}

bool SavedOptionsInterface::isButtons() const
{
    if(_controls==Buttons)
        return true;
    else
        return false;
}

bool SavedOptionsInterface::isSliders() const
{
    if(_controls==Sliders)
        return true;
    else
        return false;
}

void SavedOptionsInterface::setButtons()
{
    _controls=Buttons;
    writeOptions();
}

void SavedOptionsInterface::setSliders()
{
    _controls=Sliders;
    writeOptions();
}

ClientInfo SavedOptionsInterface::clientInfo() const
{
    return _clientInfo;
}

void SavedOptionsInterface::setClientInfo(ClientInfo ci)
{
    _clientInfo.name=ci.name;
    _clientInfo.address=ci.address;
    writeOptions();
}

QString SavedOptionsInterface::serverAddress() const
{
    return _serverAddress;
}

void SavedOptionsInterface::setServerAddress(QString &sa)
{
    _serverAddress=sa;
    writeOptions();
}
