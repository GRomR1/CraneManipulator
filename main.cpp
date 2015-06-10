#include <QApplication>
#include "BluetoothClient.h"
#include "FindDevices.h"
#include "SavedOptionsInterface.h"
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/simple.qss");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(QLatin1String(file.readAll()));
    file.close();

    SavedOptionsInterface *options = new SavedOptionsInterface;
    options->readOptions();

    BluetoothClient *client = new BluetoothClient;
    FindDevices *fd = new FindDevices(options);

    QObject::connect(fd, SIGNAL(localDeviceInfoReaded(QString,QString)),
                     client, SLOT(setLocalNameAndAddress(QString,QString)));
    QObject::connect(fd, SIGNAL(itemSelected(QString,QBluetoothAddress)),
                     client, SLOT(setAddress(QString,QBluetoothAddress)));
    QObject::connect(fd, SIGNAL(simulationMode(bool)),
                     client, SLOT(setSimulationMode(bool)));
    QObject::connect(fd, SIGNAL(selectedControls(Controls)),
                     client, SLOT(setControls(Controls)));

    QObject::connect(client, SIGNAL(showFindDevices()),
                     fd, SLOT(show()));
    QObject::connect(client, SIGNAL(showFindDevices()),
                     client, SLOT(close()));
    QObject::connect(fd, SIGNAL(showBluetoothClient()),
                     client, SLOT(show()));
    QObject::connect(fd, SIGNAL(showBluetoothClient()),
                     fd, SLOT(close()));

    if(options->keepIsMind())
    {
        ClientInfo ci=options->clientInfo();
        client->setLocalNameAndAddress(ci.name, ci.address);
        client->setControls(options->controls());
        client->setSimulationMode(options->simulation());
        if(!options->simulation())
        {
            client->setAddress(options->serverName(),
                        QBluetoothAddress(options->serverAddress()));
        }
        client->show();
    }
    else
        fd->show();

    return a.exec();
}
