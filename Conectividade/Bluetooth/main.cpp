/*------------------------------------------------------------------------------------------------

 Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .

------------------------------------------------------------------------------------------------*/

#include "bluetoothdevice.h"
#include "bluetoothDeviceAdaptor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    new BluetoothDevice();

    return a.exec();
}
