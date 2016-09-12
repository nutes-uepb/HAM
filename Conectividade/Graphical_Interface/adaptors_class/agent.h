/*------------------------------------------------------------------------------------------------

# Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .
# Licensed under the MIT license;
# You may obtain a copy of the License at:
# http://opensource.org/licenses/mit-license.php or see LICENSE file

------------------------------------------------------------------------------------------------*/

#ifndef AGENT_H
#define AGENT_H
#include <QString>
#include <QDBusInterface>
#include <QObject>
#include <QDebug>
#include <QDBusObjectPath>

class Agent : public QObject
{
    Q_OBJECT
public:
    explicit Agent(QObject *parent = 0);
    void setRequestAttributes(bool value);

    void setRequestConfiguratiosn(bool value);

public slots:
    void Connected(QString device, QString address);
    void Associated(QString device, QString xmlData);
    void MeasurementData(QString device, QString xmlData);
    void DeviceAttributes(QString device, QString xmlData);
    void Disassociated(QString device);
    void Disconnected(QString device);

signals:
    void deviceConnected(QString address);
    void deviceDisconnected();
    void currentStatus(QString);
    void xmlMeasures(QString);
    void xmlConfigurations(QString);
    void xmlDeviceAttributes(QString);

private:
    bool requestAttributes, requestConfiguratiosn;

};

#endif // AGENT_H
