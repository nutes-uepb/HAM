/**
  * @file interface.h
  * @version 1.0
  * @date 23/11/2015
  * @author NUTES
  * @title  Graphical Interface Program
  * @brief Methods used on building Graphical Interface Program
  */

#ifndef INTERFACE_H
#define INTERFACE_H

#include"libraries.h"
#include <QSplitter>
#include <QBluetoothLocalDevice>
#include "adaptors_class/agent.h"
#include "xml/measurement.h"
#include "xml/configuration.h"
#include "xml/deviceattributes.h"

#define DbPath "/home/lucas/Documentos/Conectividade/conectividade.db"


namespace Ui {
class Interface;
}
/**
 * @brief The Interface class
 */
class Interface : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Interface is the default constructor for this class
     * @param parent
     */
    explicit Interface(QWidget *parent = 0);
    /**
      * @brief Destructor the class Interface
      */
    ~Interface();

private: //Methods
    void antidoteConfigurations();
    void activeAgentDbus();
    void settingUpInterface();
    void Connecting();
    void loadSavedData();

private slots:
    /**
     * @brief Notify the object Interface when it's connect with bluetooth module
     * @param event
     */
    void timerEvent(QTimerEvent *event);
    void bus_nameownerchanged(QString service,QString old, QString news);
    void enable_Module_Bluetooth();
    void disable_Module_Bluetooth();
    void enable_actionAntidote();
    void disable_actionAntidote();
    void on_actionSair_triggered();
    void connectedUser(QPair<int,QString> user);
    /**
     * @brief on_pushButtonScan_clicked is executed when the user interface clicked in the "Scan" Buttom, initializing the search for
     * hdp device
     */
    void on_pushButtonScan_clicked();
    /**
     * @brief devices_discovery_received will show the newly discovered device on list of discovered devices
     * @param actualName is the name of the newly discovered device
     * @param actualAddress is the address of the newly discovered device
     */
    void devices_discovery_received(const QString &actualName, const QString &actualAddress, bool statusPairing);
    void stopingSearch();
    /**
     * @brief scanFinished will define the layout interface when the search emit a signal to notify the final search
     */
    void scanFinished() const;
    /**
     * @brief on_pushButtonPair_clicked is executed when the user interface clicked in the "Pair" Buttom, initializing the pairing with
     * a hdp device
     */
    void on_pushButtonPair_clicked();
    /**
     * @brief show_pin show a message with a PIN for the other device confirms pairing
     * @param actualName is the name of device that user interface wants pair
     * @param pin is the code for confirms pairing
     */
    void show_pin(const QString &actualName, const QString &pin);

    /**
     * @brief errorPairing show a message of error if it happen some error during the paring
     */
    void errorPairing();
    /**
     * @brief devices_paired_received will show the newly paired device on list of paired devices
     * @param name is the name of the newly paired device
     * @param address is the address of the newly paired device
     */
    void devices_paired_received(const QString address);
    /**
     * @brief displayPairingMenu is the sub menu that used when the user interface whants unpair a device
     * that it's on list of paried devices
     * @param pos
     */
    void displayUnpairingMenu(const QPoint &pos);
    /**
     * @brief setting_attributes set attributes name and address as the currently selected device
     * @param item is the device currently selected
     */
    void setting_attributes(QListWidgetItem*item);
    void on_checkDeviceConfigurations_clicked(bool checked);
    void on_checkDeviceAttributes_clicked(bool checked);
    void labelConnect(QString address);
    void labelDisconnect();
    void currentStatus(QString status);
    void showDeviceAttributes(QString xmlAttributes);
    void showDeviceConfiguration(QString xmlConfiguration);
    void showDeviceMeasures(QString xmlReceived);
    void on_pushButtonSave_clicked();

    void on_listWidgetData_doubleClicked(const QModelIndex &index);

    void on_pushButtonGoBack_clicked();

    void on_pushButtonClear_clicked();



private: //Attributes
    QString currentName;
    QString currentAddress;
    QString modelCurrentDevice;
    QString timestampCurrentMeasurement;
    QList<QPair<QString,QString> >foundDevices;
    QList<QPair<QString,QString> >pairedDevices;
    conectividade::BluetoothDevice *bluetoothDevice;
    Ui::Interface *ui;
    QProcess *processBluetooth, *processAntidote;
    Agent *agt;
    int userCode;
    QSqlDatabase myDB;
    QStringList tables;

    //Measurement *meditions;
};

#endif // INTERFACE_H
