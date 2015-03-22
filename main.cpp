#include <QApplication>
#include "BluetoothClient.h"
#include "FindDevices.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FindDevices *fd = new FindDevices;
    BluetoothClient *client = new BluetoothClient;
    QObject::connect(fd, SIGNAL(localDeviceInfoReaded(QString,QString)), client, SLOT(setLocalNameAndAddress(QString,QString)));
    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)), fd, SLOT(close()));
    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)), client, SLOT(show()));
    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)), client, SLOT(setAddress(QBluetoothAddress)));

    QObject::connect(client, SIGNAL(showFindDevices()), fd, SLOT(show()));

#ifdef DEBUG
    client->show();
#else
    fd->show();
#endif

    return a.exec();
}
