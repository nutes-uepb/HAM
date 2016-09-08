/*------------------------------------------------------------------------------------------------

 Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .

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
