#ifndef LIBRARIES
#define LIBRARIES

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QMovie>
#include <QFileDialog>
#include <QInputDialog> //mudar include
#include "interfaces_class/bluetoothDeviceInterface.h"
#include <QtSql>

#define SERVICE_NAME "org.nutes.signove"
#define PATH_OBJECT "/org/nutes/signove"
#define CONNECTION QDBusConnection::sessionBus()

#endif // LIBRARIES

