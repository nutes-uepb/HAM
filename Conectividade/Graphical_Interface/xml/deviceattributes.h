#ifndef DEVICEATTRIBUTES_H
#define DEVICEATTRIBUTES_H

#include "xml/xml.h"
#include <vector>
class DeviceAttributes
{
public:
    DeviceAttributes(DataList datalist);
    QList<QPair<QString, QString> > describe();
private:
    QList<QPair<QString, QString> > system_model(Entry entry);
    QPair<QString, QString> system_id(Entry entry);
    QPair<QString,QString> system_type(Entry entry);
private:
    DataList *data;
    QList<void(*)()> handlers;
};

#endif // DEVICEATTRIBUTES_H
