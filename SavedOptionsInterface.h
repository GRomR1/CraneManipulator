#ifndef SAVEDOPTIONSINTERFACE_H
#define SAVEDOPTIONSINTERFACE_H

#include "../Defines.h"
#include <QFile>
#include <QtSql>


#define SHOW_DEBUG

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

    void readOptions();
    void writeOptions();

private:
    bool _keepInMind;
    bool _simulation;
    QString _serverAddress;
    ClientInfo _clientInfo;
    Controls _controls;

    bool createConnection()
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("client_options");

        db.setUserName("root");
        db.setHostName("localhost");
        db.setPassword("password");
        if (!db.open()) {
            qDebug() << "Cannot open database:" << db.lastError().text();
            return false;
        }
        return true;
    }
};

#endif // SAVEDOPTIONSINTERFACE_H
