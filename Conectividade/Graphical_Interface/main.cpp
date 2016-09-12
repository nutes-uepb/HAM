/*------------------------------------------------------------------------------------------------

# Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .
# Licensed under the MIT license;
# You may obtain a copy of the License at:
# http://opensource.org/licenses/mit-license.php or see LICENSE file

------------------------------------------------------------------------------------------------*/

#include <QApplication>
#include <QDebug>
#include "adaptors_class/agent.h"
#include "adaptors_class/agentAdaptor.h"
#include "LoginDialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Determinando o idioma do programa
    bool confirmation;

    QTranslator T;

    QStringList languages;
    languages << "Portuguese" <<"English";
    const QString language = QInputDialog::getItem(NULL,"Language","Select Language", languages,0,true,&confirmation);
    //Quando a mudança de linguagem for feita

    if(!confirmation)
        return 0;

    if(language=="Portuguese"){
        T.load(":/portugues.qm");
        a.installTranslator(&T);
    }

    LoginDialog l;
    l.show();

    return a.exec();
}

