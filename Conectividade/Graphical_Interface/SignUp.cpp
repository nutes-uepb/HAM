#include "SignUp.h"
#include "ui_SignUp.h"
#include <QMessageBox>

#define DbPath "/home/lucas/Documentos/Conectividade/conectividade.db"

SignUp::SignUp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUp)
{
    ui->setupUi(this);

    ui->lineEditOtherID->setVisible(false);

    ui->lineEditCodPhone->setMaxLength(3);
    ui->lineEditCodPhone->setInputMask("(999)");

    ui->lineEditID->setMaxLength(11);
    ui->lineEditID->setInputMask("99999999999");

    ui->lineEditZipCode->setMaxLength(9);
    ui->lineEditZipCode->setInputMask("999999999");

    ui->lineEditPhone->setMaxLength(10);
    ui->lineEditPhone->setInputMask("9999999999");

    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName(DbPath);
}

SignUp::~SignUp()
{
    myDB.close();
    emit SignUpClosed();
    delete ui;
}

void SignUp::on_pushButtonRegister_clicked()
{
    if (!testempty() ||
        !testmail() ||
        !testpassword()) return;

    myDB.open();

    QSqlQuery query;

    query.exec("SELECT code FROM login");
    int lastcode;
    while (query.next())
    {
        lastcode = query.value(0).toInt();
    }

    qDebug() << lastcode;

    query.prepare("INSERT INTO login (code, username, password) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(lastcode+1);
    query.addBindValue(ui->lineEditUsername->text());
    query.addBindValue(ui->lineEditPassword->text());
    query.exec();

    query.prepare("INSERT INTO personalInfo (Code, Name, Address, Phone,  Email, Birthday, ID) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(lastcode+1);
    query.addBindValue(ui->lineEditName->text());
    query.addBindValue(ui->lineEditAddress->text());
    query.addBindValue(ui->lineEditCodPhone->text() + ui->lineEditPhone->text());
    query.addBindValue(ui->lineEditEmail->text());
    query.addBindValue(QString::number(ui->spinBoxDayBDay->value()) + " " + ui->comboBoxMonthBDay->currentText() + " " + QString::number(ui->spinBoxYearBDay->value()));
    query.addBindValue(ui->comboBox->currentText() + " " + ui->lineEditOtherID->text() + ":" + ui->lineEditID->text());
    query.exec();

    QMessageBox::information(this,"WELCOME!", "Registered successfully!");

    this->close();

    qDebug() << query.lastError().text();
}

void SignUp::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "Other")
    {
        ui->comboBox->setVisible(false);
        ui->lineEditOtherID->setVisible(true);
        ui->lineEditOtherID->setFocus();
    }
}

void SignUp::on_pushButtonCancel_clicked()
{
    this->close();
}

bool SignUp::testmail()
{
    int indexAt;
    bool invalidEmail = true;
    for (int i = 0; i < ui->lineEditEmail->text().length(); i++)
    {
        if (ui->lineEditEmail->text().at(i) == '@')
        {
            indexAt = i;
            invalidEmail = false;
            break;
        }
    }
    if (!invalidEmail)
    {
        invalidEmail = true;
        for (int j = indexAt; j < ui->lineEditEmail->text().length(); j++)
        {
            if (ui->lineEditEmail->text()[j] == '.')
            {
                invalidEmail = false;
                break;
            }
        }
    }
    if(invalidEmail)
    {
        QMessageBox::information(this,"ERROR","Not a valid e-mail!");
        return false;
    }
    return true;
}

bool SignUp::testempty()
{
    if (ui->lineEditAddress->text().isEmpty() ||
        ui->lineEditCity->text().isEmpty() ||
        ui->lineEditCodPhone->text().isEmpty() ||
        ui->lineEditConfirmPassword->text().isEmpty() ||
        ui->lineEditEmail->text().isEmpty() ||
        ui->lineEditID->text().isEmpty() ||
        ui->lineEditName->text().isEmpty() ||
        ( ui->comboBox->currentText() == "Other" && ui->lineEditOtherID->text().isEmpty() ) ||
        ui->lineEditPassword->text().isEmpty() ||
        ui->lineEditPhone->text().isEmpty() ||
        ui->lineEditState->text().isEmpty() ||
        ui->lineEditUsername->text().isEmpty() ||
        ui->lineEditZipCode->text().isEmpty()
        )
    {
        QMessageBox::information(this, "ERROR", "One or more fields were left empty!");
        return false;
    }
    return true;
}

bool SignUp::testpassword()
{
    if (ui->lineEditPassword->text() != ui->lineEditConfirmPassword->text())
    {
        QMessageBox::information(this,"ERROR","Passwords don't match!");
        ui->lineEditPassword->setText("");
        ui->lineEditConfirmPassword->setText("");
        return false;
    }
    return true;
}
