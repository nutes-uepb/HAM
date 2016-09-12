/*------------------------------------------------------------------------------------------------

# Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .
# Licensed under the MIT license;
# You may obtain a copy of the License at:
# http://opensource.org/licenses/mit-license.php or see LICENSE file

------------------------------------------------------------------------------------------------*/

#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class SignUp;
}

class SignUp : public QDialog
{
    Q_OBJECT

public:
    explicit SignUp(QWidget *parent = 0);
    ~SignUp();

private slots:
    void on_pushButtonRegister_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButtonCancel_clicked();

signals:
    void SignUpClosed();

private:
    Ui::SignUp *ui;
    QSqlDatabase myDB;
    bool testmail();
    bool testempty();
    bool testpassword();
};

#endif // SIGNUP_H
