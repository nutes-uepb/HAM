/*------------------------------------------------------------------------------------------------

# Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .
# Licensed under the MIT license;
# You may obtain a copy of the License at:
# http://opensource.org/licenses/mit-license.php or see LICENSE file

------------------------------------------------------------------------------------------------*/

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
