#include "configuration.h"

Configuration::Configuration(DataList datalist)
{
    data = &datalist;
}

QList<QPair<QString,QString>> Configuration::describe()
{
    QList<QPair<QString,QString>> results;
    foreach(Entry entry, data->getEntries()){
        QPair<QString,QString>aux;
        aux.first = entry.getName();
        if(entry.getName() == "Numeric")
            aux.second = entry.getEntries_map().value("Unit-Code").getValue();
        else
            if(entry.getName() == "PM-Store")
                aux.second = entry.getMeta().value("HANDLE");
        results.append(aux);
    }
    return results;
}

