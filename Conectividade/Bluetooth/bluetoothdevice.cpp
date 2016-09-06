#include "bluetoothdevice.h"

BluetoothDevice::BluetoothDevice()
{
    discoveryDevice = new QBluetoothDeviceDiscoveryAgent();
    discoveryService= new QBluetoothServiceDiscoveryAgent();
    device = new QBluetoothLocalDevice();

    connect(discoveryDevice,&QBluetoothDeviceDiscoveryAgent::canceled,[&](){emit finishedSearch();});
    connect(discoveryDevice,&QBluetoothDeviceDiscoveryAgent::finished,[&](){emit finishedSearch();});
    connect(discoveryDevice,SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),this,SLOT(analyzingTypeSearch(QBluetoothDeviceInfo)));
    connect(discoveryDevice,SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),this,SLOT(emitSearchError()));

    connect(discoveryService,SIGNAL(finished()),this,SLOT(beginSearchService()));
    connect(discoveryService,SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),this,SLOT(serviceDiscovered(QBluetoothServiceInfo)));

    connect(device,SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)),this,SLOT(emitStatusPairing(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
    connect(device,SIGNAL(error(QBluetoothLocalDevice::Error)),this,SLOT(emitErroDuringPairing(QBluetoothLocalDevice::Error)));
    connect(device,SIGNAL(pairingDisplayPinCode(QBluetoothAddress,QString)),this,SLOT(emitDisplayPin(QBluetoothAddress,QString)));
    connect(device,SIGNAL(pairingDisplayConfirmation(QBluetoothAddress,QString)),this,SLOT(emitPairingConfirmation(QBluetoothAddress,QString)));

    initializeDbusObject();
}

void BluetoothDevice::beginSearch(bool status_search_hdp)
{
    searchHdp = status_search_hdp;
    initializeStop = false;
    discoveryDevice->start();
}

bool BluetoothDevice::stopSearch()
{
    if(!initializeStop){
        discoveryDevice->stop();
        initializeStop = true;
        return true;
    }
    return false;
}

void BluetoothDevice::pairDevice(QString address)
{
    device->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    device->requestPairing(QBluetoothAddress(address),QBluetoothLocalDevice::AuthorizedPaired);
    device->pairingConfirmation(true);
}

void BluetoothDevice::unpairDevice(QString address)
{
    device->requestPairing(QBluetoothAddress(address),QBluetoothLocalDevice::Unpaired);
}

void BluetoothDevice::pairingAcceptation(bool confirmation)
{
    device->pairingConfirmation(confirmation);
}

void BluetoothDevice::emitSearchError()
{
    emit searchError();
}

void BluetoothDevice::analyzingTypeSearch(QBluetoothDeviceInfo info)
{
    if(!searchHdp){
        emitDeviceDiscovered(info.name(),info.address().toString());
    }else{
        queueDiscoveryDevice.enqueue(info);

        if(!discoveryService->isActive())
            beginSearchService();
    }
}

void BluetoothDevice::emitDeviceDiscovered(QString name, QString address)
{
        if(device->pairingStatus(QBluetoothAddress(address))==QBluetoothLocalDevice::Unpaired)
            emit foundDevice(name,address,false);
        else
            emit foundDevice(name,address,true);
}

void BluetoothDevice::beginSearchService()
{
    if(queueDiscoveryDevice.size()>0){
        discoveryService->setRemoteAddress(queueDiscoveryDevice.dequeue().address());
        discoveryService->clear();
        discoveryService->start();
    }else
        return;
}

void BluetoothDevice::serviceDiscovered(QBluetoothServiceInfo infoService)
{
    QString name, address = infoService.device().address().toString();

    foreach(QBluetoothDeviceInfo device, discoveryDevice->discoveredDevices())
        if(device.address().toString()==address)
            name = device.name();

    foreach(QBluetoothUuid uuidClassService, infoService.serviceClassUuids()){
        if(uuidClassService==QBluetoothUuid::HDPSource)
            emitDeviceDiscovered(name,address);
    }
}

void BluetoothDevice::emitStatusPairing(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing)
{
    if(pairing==QBluetoothLocalDevice::Unpaired)
        emit finishedUnpairedDevice(address.toString());
    else
        emit finishedPairedDevice(address.toString());
}

void BluetoothDevice::emitErroDuringPairing(QBluetoothLocalDevice::Error error)
{
    Q_UNUSED(error);
    emit pairError();
}

void BluetoothDevice::emitDisplayPin(QBluetoothAddress address, QString pin)
{
    emit displayPin(address.toString(),pin);
}

void BluetoothDevice::emitPairingConfirmation(QBluetoothAddress address, QString pin)
{
    emit pairingConfirmation(address.toString(),pin);
}

void BluetoothDevice::initializeDbusObject()
{
    new BluetoothDeviceAdaptor(this);

    QDBusConnection conexao = QDBusConnection :: sessionBus();
    conexao.registerService("org.nutes.signove");

    conexao.registerObject("/org/nutes/signove",this);
}
