/*------------------------------------------------------------------------------------------------

 Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .

------------------------------------------------------------------------------------------------*/

#include "interface.h"
#include "ui_interface.h"
#include "adaptors_class/agent.h"
#include "adaptors_class/agentAdaptor.h"

//Construtor da Classe
Interface::Interface(QWidget *parent) :
    QMainWindow(parent),
    bluetoothDevice(new conectividade::BluetoothDevice(SERVICE_NAME,PATH_OBJECT, CONNECTION ,this)),
    ui(new Ui::Interface)
{
    //Aqui está sendo feito a habilitação apenas de botões que serão necessários em primeira instância e setando a lista
    //de dispositivos pareados anteriormente.
    ui->setupUi(this);

    antidoteConfigurations();

    activeAgentDbus();

    settingUpInterface();

    ui->tabWidgetDisplays->setCurrentIndex(0);
    ui->pushButtonGoBack->setVisible(false);
    ui->plainTextEditData->setVisible(false);

    Connecting();

    startTimer(1000);

}

//Descontrutor da Classe
Interface::~Interface()
{
    delete ui;
}

//Conectando a Interface ao modulo Bluetooth
void Interface::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    static bool acess=false;

    if(bluetoothDevice->isValid() && acess==false){
        acess=true;
        on_pushButtonScan_clicked();
    }
    if(bluetoothDevice->isValid())
        ui->actionModule_Bluetooth->setText(tr("BT Module Connected"));
    else{
        ui->actionModule_Bluetooth->setText(tr("BT Module Disconnected"));
        settingUpInterface();
        acess = false;
    }
}

void Interface::antidoteConfigurations()
{

    QDBusConnection conection = QDBusConnection::systemBus();

    //First parameter
    QString path("/com/signove/health/agent");
    path.append(QString::number(QApplication::applicationPid()));
    QDBusObjectPath object(path);

    //Second parameter
    QList<int> types;
    types << 0x1004 << 0x1007 << 0x1029 << 0x100f;

    QDBusInterface remoteApp("com.signove.health", "/com/signove/health","com.signove.health.manager",conection);

    remoteApp.call("ConfigurePassive",QVariant::fromValue(object),QVariant::fromValue(types)).errorMessage();
}

void Interface::activeAgentDbus()
{
    QString path("/com/signove/health/agent");
    path.append(QString::number(QApplication::applicationPid()));

    agt = new Agent();
    new AgentAdaptor(agt);

    QDBusConnection::systemBus().registerObject(path,QString("com.signove.health.agent"),agt);
}

void Interface::settingUpInterface()
{
    ui->plainTextEditData->setTextInteractionFlags(Qt::TextBrowserInteraction);
    //
    ui->listOfPairedDevices->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->listOfFoundDevices->clear();
    ui->listOfPairedDevices->clear();
   // ui->actionAntidote->setVisible(false);

    ui->pushButtonPair->setEnabled(false);
    ui->pushButtonScan->setEnabled(false);
    ui->pushButtonScan->setDefault(true);
    ui->labelScanning->setVisible(false);

    ui->labelBusy->setVisible(false);
    ui->labelStatus->setVisible(false);

    ui->labelBusyStatus->setVisible(false);

    ui->toolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    ui->toolBar->setMovable(false);

    timestampCurrentMeasurement = "No date information received";
    modelCurrentDevice = "No model information received";

//    ui->pushButtonGoBack->setVisible(false);
//    ui->plainTextEditData->setVisible(false);
}

void Interface::Connecting()
{
    connect(ui->listOfFoundDevices,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(setting_attributes(QListWidgetItem*)));
    connect(ui->listOfPairedDevices,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(setting_attributes(QListWidgetItem*)));
    connect(ui->listOfPairedDevices, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayUnpairingMenu(QPoint)));

    //Buttom Scan
    connect (bluetoothDevice, SIGNAL(foundDevice(QString,QString,bool)),this, SLOT(devices_discovery_received(QString,QString,bool)));
    connect (bluetoothDevice, SIGNAL(finishedSearch()), this, SLOT(scanFinished()));

    //Buttom Pair
    connect (bluetoothDevice,SIGNAL(finishedPairedDevice(QString)),this,SLOT(devices_paired_received(QString)));
    connect (bluetoothDevice,SIGNAL(pairError()),this, SLOT(errorPairing()));
    connect(bluetoothDevice,SIGNAL(displayPin(QString,QString)),this,SLOT(show_pin(QString,QString)));

    //Buttom Connect
    connect (agt, SIGNAL(deviceConnected(QString)), this, SLOT(labelConnect(QString)));
    connect (agt, SIGNAL(deviceDisconnected()),this, SLOT(labelDisconnect()));

    //Status Bar
    connect (agt, SIGNAL(currentStatus(QString)),this,SLOT(currentStatus(QString)));

    //XML
    connect (agt, SIGNAL(xmlMeasures(QString)), this, SLOT (showDeviceMeasures(QString)));
    connect (agt, SIGNAL(xmlConfigurations(QString)), this, SLOT (showDeviceConfiguration(QString)));
    connect (agt, SIGNAL(xmlDeviceAttributes(QString)), this, SLOT (showDeviceAttributes(QString)));

    //Action Interface Bluetooth
    connect(ui->actionModule_Bluetooth,SIGNAL(triggered(bool)),this,SLOT(enable_Module_Bluetooth()));

    //Action Antidote
    connect(ui->actionAntidote,SIGNAL(triggered(bool)),this,SLOT(enable_actionAntidote()));

    //Connecting Healthd
    QDBusConnection::systemBus().connect("org.freedesktop.DBus","/org/freedesktop/DBus","org.freedesktop.DBus","NameOwnerChanged",this,SLOT(bus_nameownerchanged(QString,QString,QString)));
}

void Interface::loadSavedData()
{
    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName(DbPath);
    myDB.open();

    QSqlQuery query;

    query.exec ("SELECT name FROM sqlite_master WHERE type='table'");

    tables.clear();

    while(query.next())
    {
        if (query.value(0).toString() != "healthData" &&
            query.value(0).toString() != "login" &&
            query.value(0).toString() != "personalInfo")
            tables.append(query.value(0).toString());
    }

    qDebug() << tables;

    ui->listWidgetData->clear();

    foreach(QString table, tables)
    {
        query.exec ("SELECT code, data, time FROM \'" + table + "\' WHERE code="+ QString::number(userCode));
        while(query.next())
        {
            qDebug() << query.value(2).toString();
            ui->listWidgetData->addItem(query.value(2).toString() + "   " + table);
        }
    }

    qDebug() << query.lastError();
}

void Interface::bus_nameownerchanged(QString service, QString old, QString news)
{
    if (service == "com.signove.health"){
            if (old == "" and news != ""){
                ui->actionAntidote->setIconText("Connected Antidote");
                antidoteConfigurations();
            }else
                ui->actionAntidote->setIconText("Disconnected Antidote");
    }
                
}

void Interface::enable_Module_Bluetooth()
{
    connect(ui->actionModule_Bluetooth,SIGNAL(triggered(bool)),this,SLOT(disable_Module_Bluetooth()));
    disconnect(ui->actionModule_Bluetooth,SIGNAL(triggered(bool)),this,SLOT(enable_Module_Bluetooth()));

    processBluetooth = new QProcess(this);

    QString file = "./Bluetooth";

    processBluetooth->start(file);
}

void Interface::disable_Module_Bluetooth()
{
    disconnect(ui->actionModule_Bluetooth,SIGNAL(triggered(bool)),this,SLOT(disable_Module_Bluetooth()));
    connect(ui->actionModule_Bluetooth,SIGNAL(triggered(bool)),this,SLOT(enable_Module_Bluetooth()));

    processBluetooth->kill();
}

void Interface::enable_actionAntidote()
{
    processAntidote = new QProcess(this);
    processAntidote->start("./healthd");

    connect(ui->actionAntidote,SIGNAL(triggered(bool)),this,SLOT(disable_actionAntidote()));
    disconnect(ui->actionAntidote,SIGNAL(triggered(bool)),this,SLOT(enable_actionAntidote()));  
}

void Interface::disable_actionAntidote()
{
    disconnect(ui->actionAntidote,SIGNAL(triggered(bool)),this,SLOT(disable_actionAntidote()));
    connect(ui->actionAntidote,SIGNAL(triggered(bool)),this,SLOT(enable_actionAntidote()));

    processAntidote->kill();


}

void Interface::on_actionSair_triggered()
{
    processBluetooth->kill();
    processAntidote->kill();
    close();
}

void Interface::connectedUser(QPair<int, QString> user)
{
    ui->statusbar->showMessage("Connected as " + user.second, 0);
    userCode = user.first;
    qDebug() << "User: " + user.second + " Code: " + QString::number(user.first);

    loadSavedData();
}

//Iniciando a busca por dispositivos proximos para pareamento
void Interface::on_pushButtonScan_clicked()
{
    //Quando o botão de escaneamento for cliclado
    ui->listOfFoundDevices->clear();
    ui->listOfPairedDevices->clear();

    foundDevices.clear();
    pairedDevices.clear();

    if(ui->checkBox_hdp_devices->isChecked())
        bluetoothDevice->beginSearch(true);
    else
        bluetoothDevice->beginSearch(false);

    disconnect(ui->pushButtonScan,SIGNAL(clicked(bool)),this,SLOT(on_pushButtonScan_clicked()));
    connect(ui->pushButtonScan,SIGNAL(clicked(bool)),this,SLOT(stopingSearch()));

    ui->pushButtonScan->setText(tr("Stop Scan"));
    ui->labelScanning->setText(tr("Scanning..."));
    ui->labelStatus->setText(tr("Checking status..."));

    ui->labelBusy->setMovie(new QMovie(":/images/busy.gif"));
    ui->labelBusy->movie()->start();
    ui->labelBusy->setVisible(true);

    ui->labelBusyStatus->setMovie(new QMovie(":/images/busy.gif"));
    ui->labelBusyStatus->movie()->start();
    ui->labelBusyStatus->setVisible(true);

    ui->labelScanning->setVisible(true);
    ui->labelStatus->setVisible(true);

    ui->listOfPairedDevices->setEnabled(false);
    ui->pushButtonScan->setEnabled(true);
    ui->pushButtonClear->setEnabled(false);
    ui->pushButtonPair->setEnabled(false);
    ui->pushButtonSave->setEnabled(false);

    ui->pushButtonScan->setDefault(false);
    ui->pushButtonPair->setDefault(true);
}

//Colocando os dispositivos atualmente descoberto na lista dos dispositovos encontrado
void Interface::devices_discovery_received(const QString &name, const QString &address, bool statusPairing)
{
    static QString device;
    //Dispositivos recebidos da descoberta
    if (name != device || ui->listOfFoundDevices->count()==0)
    {
        if(statusPairing){
            pairedDevices.push_back(QPair<QString,QString>(name,address));
            ui->listOfPairedDevices->addItem((name + " (" + address + ")"));
            ui->listOfPairedDevices->item(ui->listOfPairedDevices->count()-1)->setTextColor(Qt::green);
        }else{
            ui->listOfFoundDevices->addItem(name + "(" + address+ ")");
            foundDevices.push_back(QPair<QString,QString>(name,address));
        }
    }
    device = name;
}

void Interface::stopingSearch()
{
    ui->pushButtonScan->setText(tr("Stoping Scan ..."));
    ui->pushButtonScan->setEnabled(false);
    bluetoothDevice->stopSearch();
}

//Definindo layout da Interface ao termino da busca para descobrir dispositivos próximos
void Interface::scanFinished() const
{
    ui->pushButtonScan->setEnabled(true);

    ui->labelScanning->setText(tr("Scan Finished!"));
    ui->labelStatus->setText(tr("Status checked"));

    ui->labelBusyStatus->setPixmap(QPixmap(":/images/OK.png"));
    ui->labelBusy->setPixmap(QPixmap(":/images/OK.png"));

    ui->listOfPairedDevices->setEnabled(true);
    ui->pushButtonClear->setEnabled(true);
    ui->pushButtonScan->setEnabled(true);

    ui->pushButtonScan->setText(tr("Scan"));

    connect(ui->pushButtonScan,SIGNAL(clicked(bool)),this,SLOT(on_pushButtonScan_clicked()));
    disconnect(ui->pushButtonScan,SIGNAL(clicked(bool)),this,SLOT(stopingSearch()));

    if(ui->listOfFoundDevices->currentRow()!=-1)
        ui->pushButtonPair->setEnabled(true);
}

//Iniciando o pareamento entre dispositivos
void Interface::on_pushButtonPair_clicked()
{
    //Quando o botão de parear for clicado

    ui->labelBusy->setVisible(true);
    ui->labelBusy->setMovie(new QMovie (":/images/pairing.gif"));
    ui->labelBusy->movie()->start();

    ui->labelScanning->setVisible(true);
    ui->labelScanning->setText(tr("Pairing..."));

    ui->pushButtonScan->setEnabled(false);

    ui->pushButtonPair->setEnabled(false);

    bluetoothDevice->pairDevice(currentAddress);


}

//Estabelecendo pareamento entre os dispositivos através do PIN
void Interface::show_pin(const QString &name, const QString &pin)
{
    QMessageBox::information(this,"",tr("Pairing with ")+name+tr(". The PIN is: ")+pin);
}

//Mensagem quando acontecer um erro no pareamento entre os dispositivos
void Interface::errorPairing()
{
    ui->labelBusy->setVisible(false);
    ui->labelScanning->setVisible(false);
    ui->pushButtonScan->setEnabled(true);
    QMessageBox::information(this,"",tr("Error during pairng"));
}

//colocando o dispositivo atualmente pareado na lista dos dispositivos pareados
void Interface::devices_paired_received (const QString address)
{
    //Controle de pareamento feito
    QPixmap pixmap(":/images/OK.png");
    ui->labelBusy->setPixmap(pixmap);

    ui->labelScanning->setText(currentName + " (" + address + tr(") was paired successfully!"));

    ui->pushButtonScan->setEnabled(true);
    ui->pushButtonPair->setEnabled(false);

    pairedDevices.push_back(QPair<QString,QString>(currentName,address));
    foundDevices.takeAt(ui->listOfFoundDevices->currentRow());

    ui->listOfPairedDevices->addItem((currentName + " (" + address + ")"));
    ui->listOfPairedDevices->item(ui->listOfPairedDevices->count()-1)->setTextColor(Qt::green);
    ui->listOfFoundDevices->takeItem(ui->listOfFoundDevices->currentRow());
}

//Menu para dispareamento de um ro
void Interface::displayUnpairingMenu(const QPoint &pos)
{
    if(ui->listOfPairedDevices->count()>0){
        setting_attributes(ui->listOfPairedDevices->currentItem());
        QMenu menu(this);
        int currentIndex = ui->listOfPairedDevices->currentIndex().row();
        QAction *removePairAction = menu.addAction(tr("Remove Pairing"));
        QAction *chosenAction = menu.exec(ui->listOfPairedDevices->viewport()->mapToGlobal(pos));
        if (chosenAction == removePairAction){
            bluetoothDevice->unpairDevice(currentAddress);
            ui->listOfPairedDevices->takeItem(currentIndex);
            pairedDevices.takeAt(currentIndex);
        }
    }
}

//Definindo o nome e o endereço do dispositivo atualmente selecionado
void Interface::setting_attributes(QListWidgetItem *item){
    int currentRow;
    if(item->listWidget()->objectName()==ui->listOfFoundDevices->objectName()){
        currentRow=ui->listOfFoundDevices->currentRow();
        ui->pushButtonPair->setEnabled(true);
        ui->listOfPairedDevices->setCurrentRow(-1);
        currentName = foundDevices[currentRow].first;
        currentAddress = foundDevices[currentRow].second;
    }else{
        currentRow=ui->listOfPairedDevices->currentRow();
        ui->pushButtonPair->setEnabled(false);
        ui->listOfFoundDevices->setCurrentRow(-1);
        currentName = pairedDevices[currentRow].first;
        currentAddress = pairedDevices[currentRow].second;
    }
}

void Interface::on_checkDeviceConfigurations_clicked(bool checked)
{
    agt->setRequestConfiguratiosn(checked);
}

void Interface::on_checkDeviceAttributes_clicked(bool checked)
{
    agt->setRequestAttributes(checked);
}

void Interface::labelConnect(QString address)
{
    QString deviceName;

    ui->labelStatusConnection->setText(tr("<font color='green'><b>Connected</b></font>"));

    for (int i=0; i<= pairedDevices.count() - 1; i++)
    {
        if (address.toUpper() == pairedDevices[i].second)
        {
            deviceName = pairedDevices[i].first;
        }
    }
    ui->labelDeviceConnected->setText(deviceName + " (" + address.toUpper() + ")");
}

void Interface::labelDisconnect()
{
    ui->labelStatusConnection->setText(tr("<font color='red'><b>Disconnected</b></font>"));
    ui->labelDeviceConnected->setText(tr("No device connected"));
}

void Interface::currentStatus(QString status)
{
    ui->statusbar->showMessage(status,0);
    if (status == "Disassociating...")
    {
        ui->pushButtonSave->setEnabled(true);
    }
    if (status == "Connecting...")
    {
        ui->pushButtonSave->setEnabled(false);
    }
}

void Interface::showDeviceAttributes(QString xmlAttributes)
{
    //Alterado

    QList<QPair<QString,QString>> datas = DeviceAttributes(DataList(xmlAttributes)).describe();

    QPair<QString,QString> aux;

    foreach(aux, datas){
        if (aux.first == "Model" && aux.second != "") modelCurrentDevice = aux.second;
    }

    if(ui->checkDeviceAttributes->isChecked())
    {
    ui->textEditMeasures->append("\t-----------Attributes-----------");
    if(ui->checkShowXML->isChecked()){
        ui->textEditMeasures->append(xmlAttributes);
        ui->textEditMeasures->append("");
        return;
    }
    }

    foreach(aux, datas){
        ui->textEditMeasures->append("\n"+aux.first +": "+aux.second);
    }
}

void Interface::showDeviceConfiguration(QString xmlConfiguration)
{
    ui->textEditMeasures->append("\t-----------Configurations-----------");
    if(ui->checkShowXML->isChecked()){
        ui->textEditMeasures->append(xmlConfiguration);
        ui->textEditMeasures->append("");
        return;
    }
    QList<QPair<QString,QString>> datas = Configuration(DataList(xmlConfiguration)).describe();

    QPair<QString,QString> aux;
    foreach(aux, datas){
        if(aux.first == "Numeric"){
            ui->textEditMeasures->append(aux.first);
            ui->textEditMeasures->append("\t Unit: " +aux.second);
            ui->textEditMeasures->append("");
        }else
            if(aux.first == "PM-Store"){
                ui->textEditMeasures->append(aux.first);
                ui->textEditMeasures->append("\t Handle: " +aux.second);
                ui->textEditMeasures->append("");
            }
    }
}

void Interface::showDeviceMeasures(QString xmlReceived)
{
    ui->textEditMeasures->append("\t-----------Measurements-----------");
    if(ui->checkShowXML->isChecked()){
        ui->textEditMeasures->append(xmlReceived);
        ui->textEditMeasures->append("");
        return;
    }

    QList<QPair<QString,QString>> datas = Measurement(DataList(xmlReceived)).describe();
    qDebug()<< datas<<endl;

     QPair<QString,QString> aux;

    foreach(aux, datas){
        ui->textEditMeasures->append(aux.first+" "+aux.second);
        if (aux.first.contains("Date")) timestampCurrentMeasurement = aux.first + " " + aux.second;
    }
    ui->textEditMeasures->append("");
}

void Interface::on_pushButtonSave_clicked()
{

    QSqlQuery query;

    query.exec("CREATE TABLE IF NOT EXISTS \'" + modelCurrentDevice + "\' ( code INTEGER (5), data VAR_CHAR(1000), time VAR_CHAR(50));");
    qDebug() << "CREATE TABLE IF NOT EXISTS " + modelCurrentDevice + "( code INTEGER (5), data VAR_CHAR(1000), time VAR_CHAR(50));";
    qDebug() << query.lastError();

    query.prepare("INSERT INTO \'" + modelCurrentDevice +"\' (code, data, time) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(userCode);
    query.addBindValue(ui->textEditMeasures->toPlainText());
    query.addBindValue(timestampCurrentMeasurement);
    query.exec();
    if (query.lastError().text() == " ")
    {
        QMessageBox::information(this,"Success","Data saved successfully!");
        ui->pushButtonSave->setEnabled(false);
    }
    else
    {
        QMessageBox::information(this,"ERROR","Failed to save data!");
    }

    loadSavedData();
}

void Interface::on_listWidgetData_doubleClicked(const QModelIndex &index)
{
    QString time = ui->listWidgetData->item(index.row())->text().split("   ").at(0);
    QString model = ui->listWidgetData->item(index.row())->text().split("   ").at(1);

    QSqlQuery query;
    query.exec("SELECT data FROM \'" + model + "\' WHERE time=\'" + time + "\'");
    qDebug() << "SELECT data FROM \'" + model + "\' WHERE time=\'" + time + "\'";
    while (query.next())
    {
    ui->plainTextEditData->setPlainText(query.value(0).toString());
    }
    ui->listWidgetData->setVisible(false);
    ui->plainTextEditData->setVisible(true);
    ui->pushButtonGoBack->setVisible(true);
}

void Interface::on_pushButtonGoBack_clicked()
{
    ui->listWidgetData->setVisible(true);
    ui->plainTextEditData->setVisible(false);
    ui->pushButtonGoBack->setVisible(false);
}

void Interface::on_pushButtonClear_clicked()
{
    ui->pushButtonSave->setEnabled(false);
}


