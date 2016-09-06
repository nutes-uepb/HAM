#ifndef BLUETOOTHDEVICE_H
#define BLUETOOTHDEVICE_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>
#include "bluetoothDeviceAdaptor.h"
#include <QQueue>

class BluetoothDevice : public QObject
{
    Q_OBJECT
public:
    BluetoothDevice();

public slots:
    void beginSearch(bool status_search_hdp=false);
    bool stopSearch();
    void pairDevice(QString address);
    void unpairDevice(QString address);
    void pairingAcceptation(bool confirmation);

signals:
    void finishedSearch();
    void searchError();
    void foundDevice(const QString &name,const QString &address, const bool &situation);
    void finishedUnpairedDevice (const QString &address);
    void finishedPairedDevice(const QString &address);
    void pairError();
    void displayPin(QString address, QString pin);
    void pairingConfirmation(QString address, QString pin);

private:
    bool initializeStop, searchHdp;
    QBluetoothDeviceDiscoveryAgent *discoveryDevice;
    QBluetoothServiceDiscoveryAgent *discoveryService;
    QBluetoothLocalDevice *device;
    QQueue<QBluetoothDeviceInfo> queueDiscoveryDevice;

private slots:
    void emitSearchError();
    void analyzingTypeSearch(QBluetoothDeviceInfo info);
    void emitDeviceDiscovered(QString name, QString address);
    void beginSearchService();
    void serviceDiscovered(QBluetoothServiceInfo infoService);
    void emitStatusPairing(QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing);
    void emitErroDuringPairing(QBluetoothLocalDevice::Error error);
    void emitDisplayPin(QBluetoothAddress address, QString pin);
    void emitPairingConfirmation(QBluetoothAddress address,QString pin);
    void initializeDbusObject();
};

#endif // BLUETOOTHDEVICE_H
