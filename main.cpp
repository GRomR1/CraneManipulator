#include <QApplication>
#include "BluetoothClient.h"
#include "FindDevices.h"
#include "SavedOptionsInterface.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SavedOptionsInterface *options = new SavedOptionsInterface;
    options->readOptions();

    BluetoothClient *client = new BluetoothClient;
    FindDevices *fd = new FindDevices(options);
    QObject::connect(fd, SIGNAL(localDeviceInfoReaded(QString,QString)),
                     client, SLOT(setLocalNameAndAddress(QString,QString)));

    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)),
                     fd, SLOT(close()));
    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)),
                     client, SLOT(show()));
    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)),
                     client, SLOT(setAddress(QBluetoothAddress)));

    QObject::connect(client, SIGNAL(showFindDevices()),
                     fd, SLOT(show()));
    QObject::connect(client, SIGNAL(showFindDevices()),
                     client, SLOT(close()));

    QObject::connect(fd, SIGNAL(simulationMode(bool)),
                     client, SLOT(setSimulationMode(bool)));
    QObject::connect(fd, SIGNAL(simulationMode(bool)),
                     client, SLOT(show()));
    QObject::connect(fd, SIGNAL(simulationMode(bool)),
                     fd, SLOT(close()));
    QObject::connect(fd, SIGNAL(selectedControls(Controls)),
                     client, SLOT(setControls(Controls)));

    if(options->keepIsMind())
    {
        ClientInfo ci=options->clientInfo();
        client->setLocalNameAndAddress(ci.name, ci.address);
        client->setControls(options->controls());
        client->setSimulationMode(options->simulation());
        client->setAddress(QBluetoothAddress(options->serverAddress()));
        client->show();
    }
    else
        fd->show();

    return a.exec();
}
