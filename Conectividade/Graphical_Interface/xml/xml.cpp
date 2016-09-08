/*------------------------------------------------------------------------------------------------

 Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .

------------------------------------------------------------------------------------------------*/

#include "xml.h"

DataList::DataList(QString xml)
{

//    QFile file(xml);
//    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
//        qDebug()<<"Erro durante a abertura do arquivo"<<endl;
//        //QMessageBox::critical(this,"Error",file.errorString());
//        return;
//    }
    QDomDocument doc;
//    QString test("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<data-list><entry><meta-data><meta name=\"HANDLE\">1</meta></meta-data><compound><name>Numeric</name><entries><entry><meta-data><meta name=\"partition\">2</meta><meta name=\"metric-id\">19384</meta><meta name=\"unit-code\">544</meta><meta name=\"unit\">%</meta></meta-data><simple><name>Basic-Nu-Observed-Value</name><type>float</type><value>96.500000</value></simple></entry></entries></compound></entry><entry><meta-data><meta name=\"HANDLE\">10</meta></meta-data><compound><name>Numeric</name><entries><entry><meta-data><meta name=\"partition\">2</meta><meta name=\"metric-id\">18458</meta><meta name=\"unit-code\">2720</meta><meta name=\"unit\">bpm</meta></meta-data><simple><name>Basic-Nu-Observed-Value</name><type>float</type><value>63.500000</value></simple></entry></entries></compound></entry></data-list>");
    doc.setContent(QString(xml));
    //doc.setContent(&file);
//    file.close();

    QDomElement datalist(doc.firstChildElement("data-list"));

    if(!datalist.nextSiblingElement("data-list").isNull()){
        qDebug()<<"Erro: Existe mais de uma data-list neste arquivo"<<endl;
        return ;
    }

    QDomElement entry(datalist.firstChildElement());

    while(!entry.isNull()){
        //qDebug()<<entry.tagName()<<endl;
        Entry currentEntry(entry);
        entries.append(currentEntry);
        entries_map.insert(currentEntry.getName(),currentEntry);
        entry = entry.nextSibling().toElement();

    }
}

QList<Entry> DataList::getEntries() const
{
    return entries;
}

QMap<QString, Entry> DataList::getEntries_map() const
{
    return entries_map;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Entry::Entry()
{

}

Entry::Entry(QDomElement entry)
{
    meta = parse_meta(entry);
    node = detect_type(entry);
    name = parse_name(node.second);

    if(node.first){
        QPair<QList<Entry>,QMap<QString,Entry>> sub_entries = parse_children(node.second);
        entries = sub_entries.first;
        entries_map = sub_entries.second;
        compound = true;
    }else{
        QPair<QString,QString> simple = parse_simple(node.second);
        dtype = simple.first;
        value = simple.second;
        compound = false;
    }
}

QMap<QString,QString> Entry::parse_meta(QDomElement node)
{
    QMap<QString,QString> m;
    QDomElement metas = node.firstChildElement("meta-data");

    if(!metas.nextSiblingElement("meta-data").isNull()){
        qDebug()<<"Erro: Existe mais de um meta-data"<<endl;
    }
    if(metas.isNull())
        return m;
    metas = metas.firstChildElement("meta");

    while(!metas.isNull()){
        QPair<QString,QString> nameValue = parse_metadata(metas);
        m[nameValue.first] = nameValue.second;
        metas = metas.nextSibling().toElement();
    }
    return m;
}

QPair<QString,QString> Entry::parse_metadata(QDomElement meta)
{
    QPair<QString,QString> nameValue;

    if(!meta.hasAttributes())
        qDebug()<< "Nenhum atribuno no meta data" << endl;

    nameValue.first = meta.attribute("name");

    if(nameValue.first.isEmpty())
        qDebug()<< "Nenhum nome de atributo em meta data" << endl;

    nameValue.second = meta.text();

    //qDebug()<<nameValue.first<<"  "<<nameValue.second<<endl;
    return nameValue;
}

QPair<bool, QDomElement> Entry::detect_type(QDomElement node)
{
    QPair<bool,QDomElement>fragment;
    QDomElement simple = node.firstChildElement("simple");
    QDomElement compound = node.firstChildElement("compound");

    if((!simple.nextSiblingElement("simple").isNull()) || (!compound.nextSiblingElement("compound").isNull())){
        qDebug()<< "existe maide de um simple ou compound" << endl;
    }

    fragment.first = !compound.isNull();

    if(fragment.first)
        fragment.second=compound;
    else
        fragment.second=simple;

    return fragment;
}

QString Entry::parse_name(QDomElement node)
{
    QDomElement name = node.firstChildElement("name");
    if(name.isNull()){
        qDebug() << "Não existe nenhum nome" << endl;
    }
    return name.text();
}

QPair<QList<Entry>,QMap<QString,Entry>> Entry::parse_children(QDomElement node)
{
    QPair<QList<Entry>,QMap<QString,Entry>> sub_entries;

    QDomElement entry_compound = node.firstChildElement("entries");

    if(!entry_compound.nextSiblingElement("entries").isNull()){
        qDebug() << "Compound com numero de entradas de tag errada" << endl;
    }

    QDomElement entry = entry_compound.firstChildElement();

    while(!entry.isNull()){
        //qDebug()<<entry.tagName()<<endl;
        Entry currentEntry(entry);

        sub_entries.first.append(currentEntry);
        sub_entries.second.insert(currentEntry.getName(),currentEntry);

        entry = entry.nextSibling().toElement();
    }
    return sub_entries;
}

QPair<QString,QString> Entry::parse_simple(QDomElement node)
{
    QDomElement type = node.firstChildElement("type");
    QDomElement value = node.firstChildElement("value");

    if(type.isNull()){
        qDebug()<<"Erro: type"<<endl;
    }
    if(value.isNull()){
        qDebug()<<"Erro: value"<<endl;
    }
    QPair<QString,QString> simple;
    simple.first = type.text();
    simple.second = value.text();
    return simple;
}

QString Entry::getName()
{
    return name;
}

QList<Entry> Entry::getEntries() const
{
    return entries;
}

QString Entry::getValue() const
{
    return value;
}

QMap<QString, QString> Entry::getMeta() const
{
    return meta;
}

QMap<QString, Entry> Entry::getEntries_map() const
{
    return entries_map;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

