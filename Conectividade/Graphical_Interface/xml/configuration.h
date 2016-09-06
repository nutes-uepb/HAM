#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include"xml.h"

class Configuration
{
public:
    Configuration(DataList datalist);
    QList<QPair<QString, QString> > describe();
private:
    DataList *data;
};

#endif // CONFIGURATION_H
