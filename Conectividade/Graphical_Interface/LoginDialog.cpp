/*------------------------------------------------------------------------------------------------

# Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .
# Licensed under the MIT license;
# You may obtain a copy of the License at:
# http://opensource.org/licenses/mit-license.php or see LICENSE file

------------------------------------------------------------------------------------------------*/

#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include <QDebug>
#include <QMessageBox>
#define DbPath "/home/lucas/Documentos/Conectividade/conectividade.db"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);


    s = new SignUp;
    i = new Interface();

    QStringList savedUsers = TextStream("","","read").split("**",QString::SkipEmptyParts);
    foreach(QString savedUser,savedUsers)
    {
        ui->comboBox_2->addItem(savedUser.split(":").first());
    }

    ui->comboBox_2->addItem("Other");

    if(ui->comboBox_2->currentText() == "Other")
    {
        ui->comboBox_2->setVisible(false);
    }
    else
    {
        ui->lineEdit_3->setVisible(false);
    }

    connect(this,SIGNAL(connectedUser(QPair<int,QString>)),i,SLOT(connectedUser(QPair<int,QString>)));
    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(otherUser(int)));
    connect(s,SIGNAL(SignUpClosed()),this,SLOT(SignUpClosed()));

    ui->label_7->setText("<b>Conectividade</b>");
    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName(DbPath);
    QFileInfo checkFile (DbPath);

    if(checkFile.isFile())
    {
        if (myDB.open())
        {
            qDebug() << "connected";

        }
    }
    else
    {
        qDebug() << "not connected";
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
    myDB.close();
}

void LoginDialog::on_pushButton_2_clicked()
{
    this->close();
}

void LoginDialog::otherUser(int index)
{
    qDebug() << index;
    if (index == ui->comboBox_2->count() - 1)
    {
        ui->comboBox_2->setVisible(false);
        ui->lineEdit_3->setVisible(true);
        ui->lineEdit_3->setFocus();
    }
}

void LoginDialog::SignUpClosed()
{
    myDB.open();
}

void LoginDialog::on_pushButton_clicked()
{
    QSqlQuery query;
    QString username, password;
    if(ui->comboBox_2->currentText() == "Other")
    {
        username = ui->lineEdit_3->text();
        qDebug() << "1" << query.exec("SELECT code FROM login WHERE username='" + username + "'");
        query.next();
        QString code = query.value(0).toString();
        qDebug() << "2" << query.exec("SELECT Name FROM personalInfo WHERE Code='" + code + "'");
        query.next();
        if(!TextStream("","","read").contains(username))
        {
            TextStream(username,query.value(0).toString(),"write");
        }
    }
    else
    {
        QStringList usersSaved = TextStream("","","read").split("**", QString::SkipEmptyParts);
        foreach (QString userSaved, usersSaved)
        {
            if(userSaved.contains(ui->comboBox_2->currentText()))
            {
                QStringList elements = userSaved.split(":");
                username = elements.last();
            }
        }

    }
    qDebug() << username;
    password = ui->lineEdit_4->text();
    if (!myDB.open())
    {
        qDebug() << "no connection";
        return;
    }
    else
    {
        qDebug() << "opened";
    }
    qDebug() << query.lastError().text();

    QSqlQuery qry;
    if (qry.exec("SELECT code,username,password FROM login WHERE username='"+ username +"' and password='"+ password + "'"))
    {
        if (qry.next())
        {
            qDebug() << "success";
            QString Code = qry.value(0).toString();
            qDebug() << Code;
            qDebug() << qry.exec("SELECT Name FROM personalInfo WHERE Code='"+ Code +"'");
            qry.next();
            qDebug() << qry.value(0).toString();

            QPair <int,QString> pair;
            pair.first = Code.toInt();
            pair.second = qry.value(0).toString();
            emit connectedUser(pair);

            i->showFullScreen();
            this->close();
        }
        else
        {
            QMessageBox::information(this,"ERROR", "Username or password is incorrect!");
            qDebug() << "try again";
        }
    }
    else
    {
        qDebug () << "problem";
        qDebug() << query.lastError().text();
    }

}

void LoginDialog::on_commandLinkButton_clicked()
{
    s->show();
}

QString LoginDialog::TextStream(QString username, QString name, QString whatToDo)
{
    QFile data("/home/lucas/Documentos/Conectividade/savedUsers.txt");
    if(whatToDo == "read")
    {
        data.open(QFile::ReadWrite);
        QTextStream in(&data);
        return in.readLine();
    }
    else
    {
        QStringList separatedName = name.split(" ");
        name = separatedName.first() + " " + separatedName.last();
        if (data.open(QFile::Append))
        {
            QTextStream out(&data);
            out << name << ":" << username << + "**";
            return "";
        }
    }
}
