#include "SavedOptionsInterface.h"

//void traverseNode(const QDomNode& node)
//{
//    QDomNode domNode = node.firstChild();
//    while(!domNode.isNull()) {
//        if(domNode.isElement()) {
//            QDomElement domElement = domNode.toElement();
//            if(!domElement.isNull()) {
//                if(domElement.tagName() == "Options") {
//                    qDebug() << "KeepInMind: "
//                             << domElement.attribute("KeepInMind")
//                             << "Simulation: "
//                             << domElement.attribute("Simulation");
//                }
//                if(domElement.tagName() == "Controls") {
//                    qDebug() << "KeepInMind: "
//                             << domElement.attribute("KeepInMind")
//                             << "Simulation: "
//                             << domElement.attribute("Simulation");
//                }
//                if(domElement.tagName() != "Controls" && domElement.tagName() != "Options"){
//                    qDebug() << "TagName: " << domElement.tagName()
//                             << "\tText: " << domElement.text();
//                }

//            }
//        }
//        traverseNode(domNode);
//        domNode = domNode.nextSibling();
//    }
//}

SavedOptionsInterface::SavedOptionsInterface() :
    _keepInMind(false),
    _simulation(false),
    _controls(Sliders)
{
    if (!createConnection()) {
        qDebug() << "Error connecting of database";
    }
    QSqlQuery query;
    QString   str  = "CREATE TABLE IF NOT EXISTS options ( "
                     "Id INTEGER PRIMARY KEY NOT NULL, "
                     "KeepInMind   VARCHAR(5), "
                     "Simulation  VARCHAR(5), "
                     "ServerAddress  VARCHAR(25), "
                     "ClientName  VARCHAR(30), "
                     "ClientAddress  VARCHAR(25), "
                     "Controls  VARCHAR(10) "
                     ");";

    if (!query.exec(str)) {
        qDebug() << "Unable to create a table";
    }
}

void SavedOptionsInterface::readOptions()
{
    QSqlQuery query;
    if (!query.exec("SELECT * FROM options;")) {
        qDebug() << "Unable to execute query - exiting";
    }
    QSqlRecord rec = query.record();
//    qDebug() << "Number of columns: " << rec.count();
    while (query.next()) {
        QString str;
        str = query.value(rec.indexOf("KeepInMind")).toString();
        if(str.compare("yes")==0) //is equal
            _keepInMind=true;
        else
            _keepInMind=false;
        str = query.value(rec.indexOf("Simulation")).toString();
        if(str.compare("yes")==0) //is equal
            _simulation=true;
        else
            _simulation=false;
        _serverAddress = query.value(rec.indexOf("ServerAddress")).toString();
        _clientInfo.name = query.value(rec.indexOf("ClientName")).toString();
        _clientInfo.address = query.value(rec.indexOf("ClientAddress")).toString();
        str = query.value(rec.indexOf("Controls")).toString();
        if(str.compare("buttons")==0) //is equal
            _controls=Buttons;
        else
            _controls=Sliders;

//        qDebug() << str;
    }


}

void SavedOptionsInterface::writeOptions()
{
    QSqlQuery query;
    QString strF =
            "UPDATE options "
            "SET KeepInMind='%1', "
            "Simulation='%2', "
            "ServerAddress='%3', "
            "ClientName='%4', "
            "ClientAddress='%5', "
            "Controls='%6' "
            "WHERE id=1;";

    QString str = strF.arg((_keepInMind? "yes": "no"))
            .arg((_simulation? "yes": "no"))
            .arg(_serverAddress)
            .arg(_clientInfo.name)
            .arg(_clientInfo.address)
            .arg((_controls==Sliders? "sliders": "buttons"));
    if (!query.exec(str)) {
        qDebug() << "Unable to make update opeation";
    }
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
