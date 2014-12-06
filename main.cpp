#include <QApplication>
#include "BluetoothClient.h"
#include "FindDevices.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FindDevices *fd = new FindDevices;
    BluetoothClient *client = new BluetoothClient;
    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)), fd, SLOT(close()));
    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)), client, SLOT(show()));
    QObject::connect(fd, SIGNAL(addressSelected(QBluetoothAddress)), client, SLOT(setAddress(QBluetoothAddress)));

#ifdef DEBUG
    client->show();
#else
    fd->show();
#endif

    return a.exec();
}
