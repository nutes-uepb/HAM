/*------------------------------------------------------------------------------------------------

 Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .

------------------------------------------------------------------------------------------------*/

#ifndef MEASUREMENT_H
#define MEASUREMENT_H
#include "xml.h"
#include <QObject>
#include "measurementAdaptor.h"

class Measurement : public QObject
{
    Q_OBJECT

public slots:
    QString getDateMeasurement() const;
    QString getTimeMeasurement() const;
public:
    Measurement(DataList datalist, int inside_segment=0);
    QString unit(Entry entry);
    QPair<QString, QString> simple_nu(Entry entry);
    QPair<QString, QString> basic_nu(Entry entry);
    QPair<QString, QString> compound_basic_nu(Entry entry);
    QPair<QString, QString> absolute_timestamp(Entry entry);
    QList<QPair<QString, QString> > describe();

    void setDateMeasurement(const QString &value);
    void setTimeMeasurement(const QString &value);

    void initializeDbusObject();

private:
    DataList *data;
    int inside_segment;
    QList<QPair<QString,QString> > meditions;

    QString dateMeasurement;
    QString timeMeasurement;
};

#endif // MEASUREMENT_H
