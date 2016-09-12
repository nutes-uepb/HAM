/*------------------------------------------------------------------------------------------------

# Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .
# Licensed under the MIT license;
# You may obtain a copy of the License at:
# http://opensource.org/licenses/mit-license.php or see LICENSE file

------------------------------------------------------------------------------------------------*/

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
